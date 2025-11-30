//
// Physics Simulation Program
// File: step_manager.cpp
// Created by Tobias Sharman on 01/09/2025
//
// Description:
//   - Implementation of step_manager.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "simulation/stepping/step_manager.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <thread>
#include <vector>

#include "config/program_config.h"
#include "core/linear-algebra/vector.h"
#include "core/random/random_manager.h"
#include "objects/object_manager.h"
#include "particles/particle_manager.h"
#include "physics/processes/interaction_utilities.h"
#include "physics/processes/discrete/core/decay_utilities.h"
#include "physics/processes/discrete/core/interaction_sampling.h"
#include "simulation/simulation_clock.h"
#include "simulation/geometry/boundary/boundary_interactions.h"
#include "simulation/motion/particle_motion.h"
#include "simulation/stepping/step_events.h"
#include "simulation/stepping/step_utilities.h"

namespace {
    StepPoint buildStepPoint(const Particle &particle, const Object *medium) {
        StepPoint point{};
        point.position = particle.getPosition();
        point.globalTime = particle.getTime();
        point.medium = medium;
        return point;
    }

    void ensureInteractionSample(Particle &particle, const Object *mediumBeforeStep) {
        if (mediumBeforeStep == nullptr || particle.hasPendingInteractionLength()) {
            return;
        }

        const auto [process, length] = discrete_interaction::sampleInteractionEvent(particle, mediumBeforeStep);
        particle.setInteractionLengthRemaining(length, process);
    }

    void stepParticle(
        std::unique_ptr<Particle> &particle,
        const Object *detector,
        const Object *world,
        const Quantity &targetTime,
        SpawnQueue &spawned
    ) {
        if (!particle) {
            return;
        }

        if (world == nullptr) {
            throw std::runtime_error("Active world is not available for particle stepping");
        }

        const auto initialTime = particle->getTime();
        auto remainingTime = targetTime - initialTime;
        if (!std::isfinite(remainingTime.value) || remainingTime.value < 0.0) {
            remainingTime.value = 0.0;
        }
        const Object *currentMedium = step_utilities::resolveContainingMedium(world, particle->getPosition());

        if (particle->getLifetime().value > 0.0 && particle->hasDecayEnergy() && !particle->hasDecayClock()) {
            if (const auto decayTime = discrete_interaction::sampleDecayTime(*particle);
                std::isfinite(decayTime.value)) {
                particle->setDecayClock(decayTime);
            }
        }

        while (remainingTime.value > 0.0 && particle && particle->getAlive()) {
            StepPoint preStep = buildStepPoint(*particle, currentMedium);
            if (preStep.medium == nullptr) {
                particle->setAlive(false);
                break;
            }

            ensureInteractionSample(*particle, preStep.medium);

            const auto event = determineStepEvent(*particle, remainingTime, preStep, world);

            if (event.dt.value <= 0.0) {
                remainingTime.value = 0.0;
                break;
            }

            const auto travelledDistance = event.displacement.length();

            particle->consumeInteractionLength(travelledDistance);
            if (particle->hasDecayClock()) {
                particle->consumeDecayTime(event.dt);
            }

            moveParticle(*particle, event.dt, event.displacement);

            if (!particle || !particle->getAlive()) {
                return;
            }

            if (event.limiter == StepLimiter::Boundary) {
                processBoundaryResponse(*particle, event.boundaryEvent, event.displacement, travelledDistance);
            }
            else if (event.limiter == StepLimiter::Interaction || event.limiter == StepLimiter::Decay) {
                processDiscreteLimiterEvent(particle, event.limiter, world, spawned);
            }

            if (!particle || !particle->getAlive()) {
                return;
            }

            const Object *mediumAfterStep = event.postStep.medium;
            if (mediumAfterStep == nullptr) {
                mediumAfterStep = step_utilities::resolveContainingMedium(world, particle->getPosition());
            }

            if (!step_utilities::updatePostEventState(particle, detector, preStep.medium, mediumAfterStep)) {
                return;
            }

            remainingTime -= event.dt;
            currentMedium = mediumAfterStep;
        }

        if (!particle) {
            return;
        }

        if (particle->getAlive() && particle->getTime() < targetTime) {
            particle->synchroniseTime(targetTime);
        }

        if (!step_utilities::ensureParticleInsideWorld(*particle, world)) {
            return; // Unnecessary; function would finish here anyway, but the explicit return is preferred
        }
    }

void stepAll(const Object *detector, const Quantity &dt) {
    if (!Unit::hasTimeDimension(dt.unit)) {
        throw std::invalid_argument("Time step must have time dimensions");
    }
    if (!std::isfinite(dt.value) || dt.value < 0.0) {
        throw std::invalid_argument("Time step must be finite and non-negative");
        }

        const auto currentTime = simulation_clock::currentTime();
        const auto targetTime = currentTime + dt;
        const auto *world = g_objectManager.getActiveWorld("step all particles");

        if (world == nullptr) {
            throw std::runtime_error("Active world is not available while stepping particles");
        }

        step_utilities::validateDetector(detector, world);

    std::vector<std::unique_ptr<Particle>> spawnedParticles;

    {
        const auto particleHandle = g_particleManager.acquireReadHandle();
        auto &particles = particleHandle.particles();
        if (const auto particleCount = particles.size(); particleCount > 0) {
            std::vector<SpawnQueue> spawnBuffers;

            // Ignore warnings about threads; they change based on maxWorkerThreads being 0 OR >= 1
                constexpr auto requestedThreads = config::program::maxWorkerThreads;
                const auto hardwareThreads = std::max<unsigned>(1, std::thread::hardware_concurrency());
                if (requestedThreads > hardwareThreads) {
                    throw std::runtime_error("Requested worker thread count exceeds hardware_concurrency");
            }
            const std::size_t availableThreads = requestedThreads > 0 ? requestedThreads : hardwareThreads;
            const std::size_t workerCount = std::min<std::size_t>(availableThreads, particleCount);

            spawnBuffers.resize(workerCount);

            const auto runChunk = [&, targetTime](const std::size_t begin, const std::size_t end, const std::size_t threadIndex) {
                const auto previousIndex = random_manager::getThreadStreamIndex();
                random_manager::setThreadStreamIndex(threadIndex);
                for (std::size_t index = begin; index < end; ++index) {
                    stepParticle(particles[index], detector, world, targetTime, spawnBuffers[threadIndex]);
                }
                random_manager::setThreadStreamIndex(previousIndex);
            };

                std::vector<std::thread> workers;
                workers.reserve(workerCount > 0 ? workerCount - 1 : 0);

                std::size_t nextBegin = 0;
                const std::size_t baseChunk = particleCount / workerCount;
                const std::size_t remainder = particleCount % workerCount;

                for (std::size_t workerIndex = 0; workerIndex < workerCount; ++workerIndex) {
                    const std::size_t chunkSize = baseChunk + (workerIndex < remainder ? 1 : 0);
                    const std::size_t chunkEnd = nextBegin + chunkSize;
                    if (workerIndex + 1 == workerCount) {
                        runChunk(nextBegin, chunkEnd, workerIndex);
                    } else {
                        workers.emplace_back(runChunk, nextBegin, chunkEnd, workerIndex);
                    }
                    nextBegin = chunkEnd;
                }

            for (auto &thread : workers) {
                if (thread.joinable()) {
                    thread.join();
                }
            }

            for (auto &buffer : spawnBuffers) {
                for (auto &p : buffer) {
                    if (p) {
                        spawnedParticles.push_back(std::move(p));
                    }
                }
                buffer.clear();
            }
        }
    }

    if (!spawnedParticles.empty()) {
        std::vector<std::unique_ptr<Particle>> survivors;
        std::vector<std::unique_ptr<Particle>> pending;
        pending.swap(spawnedParticles);

        while (!pending.empty()) {
            SpawnQueue newSpawns;
            std::vector<std::unique_ptr<Particle>> nextPending;

            for (auto &particle : pending) {
                if (!particle) {
                    continue;
                }
                stepParticle(particle, detector, world, targetTime, newSpawns);

                if (particle && particle->getAlive()) {
                    particle->synchroniseTime(targetTime);
                    survivors.push_back(std::move(particle));
                }
            }

            for (auto &particle : newSpawns) {
                if (particle) {
                    nextPending.push_back(std::move(particle));
                }
            }

            pending.swap(nextPending);
        }

        if (!survivors.empty()) {
            g_particleManager.addParticles(std::move(survivors));
        }
    }

    simulation_clock::setTime(targetTime);

    g_particleManager.withExclusiveAccess([](auto &particles) {
        step_utilities::purgeDeadParticles(particles);
    });
}
} // namespace

void stepUntilTime(const Object *detector, const Quantity &targetTime, const Quantity &dt) {
    if (!Unit::hasTimeDimension(targetTime.unit)) {
        throw std::invalid_argument("Target time must have time dimensions");
    }
    if (!std::isfinite(targetTime.value)) {
        throw std::invalid_argument("Target time must be finite");
    }
    if (!Unit::hasTimeDimension(dt.unit)) {
        throw std::invalid_argument("Time step must have time dimensions");
    }
    if (!std::isfinite(dt.value) || dt.value <= 0.0) {
        throw std::invalid_argument("Time step must be finite and positive for stepUntilTime");
    }

    const double tolerance = std::max(
        std::abs(targetTime.value) * config::program::timeSynchronisationTolerance,
        std::numeric_limits<double>::epsilon()
    );

    auto current = simulation_clock::currentTime();

    while (current.value + tolerance < targetTime.value) {
        auto remaining = targetTime - current;

        if (!std::isfinite(remaining.value) || remaining.value <= 0.0) {
            break;
        }

        if (remaining.value > dt.value) {
            remaining = dt;
        }

        stepAll(detector, remaining);
        current = simulation_clock::currentTime();
    }
}

void stepUntilEmpty(const Object *detector, const Quantity &dt) {
    if (!Unit::hasTimeDimension(dt.unit)) {
        throw std::invalid_argument("Time step must have time dimensions");
    }
    if (!std::isfinite(dt.value) || dt.value <= 0.0) {
        throw std::invalid_argument("Time step must be finite and positive for stepUntilEmpty");
    }

    while (!g_particleManager.empty()) {
        stepAll(detector, dt);
    }
}
