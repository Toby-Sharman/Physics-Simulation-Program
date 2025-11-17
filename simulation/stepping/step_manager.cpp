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

#include <cmath>
#include <stdexcept>

#include "core/linear-algebra/vector.h"
#include "objects/object_manager.h"
#include "physics/processes/interaction_utilities.h"
#include "physics/processes/continuous/particle_continuous_interactions.h"
#include "physics/processes/discrete/core/decay_utilities.h"
#include "physics/processes/discrete/core/interaction_sampling.h"
#include "particles/particle_manager.h"
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
        const Quantity &dt
    ) {
        if (!particle) {
            return;
        }

        if (world == nullptr) {
            throw std::runtime_error("Active world is not available for particle stepping");
        }

        const auto initialTime = particle->getTime();
        auto remainingTime = dt;
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
                processDiscreteLimiterEvent(particle, event.limiter, world);
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

        if (particle->getAlive()) {
            const auto expectedTime = initialTime + dt;
            particle->synchroniseTime(expectedTime);
        }

        if (!step_utilities::ensureParticleInsideWorld(*particle, world)) {
            return; // Unnecessary; function would finish here anyway, but the explicit return is preferred
        }
    }
} // namespace

void stepAll(ParticleManager &manager, const Object *detector, const Quantity &dt) {
    const auto *world = g_objectManager.getActiveWorld("step all particles");
    if (world == nullptr) {
        throw std::runtime_error("Active world is not available while stepping particles");
    }

    step_utilities::validateDetector(detector, world);

    {
        auto particleHandle = manager.acquireReadHandle();
        auto &particles = particleHandle.particles();

        for (auto &particle : particles) {
            stepParticle(particle, detector, world, dt);
        }
    }

    manager.withExclusiveAccess([](auto &particles) {
        step_utilities::purgeDeadParticles(particles);
    });
}
