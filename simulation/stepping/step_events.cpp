//
// Physics Simulation Program
// File: step_events.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of step_events
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "simulation/stepping/step_events.h"

#include <cmath>
#include <limits>
#include <stdexcept>

#include "physics/processes/interaction_utilities.h"
#include "physics/processes/continuous/particle_continuous_interactions.h"
#include "physics/processes/discrete/core/interaction_process_registry.h"
#include "simulation/stepping/step_utilities.h"

namespace {
    StepEvent makeTimeLimitedEvent(
        const Particle &particle,
        const Quantity &remainingTime,
        const StepPoint &preStep
    ) {
        StepEvent event{};
        event.limiter = StepLimiter::Time;
        event.dt = remainingTime;
        event.preStep = preStep;
        event.displacement = displacement(particle, remainingTime);
        return event;
    }

    bool shouldReplaceEvent(
        const StepEvent &baseEvent,
        const StepLimiter limiter,
        const Quantity &candidateDt
    ) {
        if (!std::isfinite(candidateDt.value) || candidateDt.value < 0.0) {
            return false;
        }

        const double baseDt = baseEvent.dt.value;
        const double difference = candidateDt.value - baseDt;
        const double magnitude = std::max(std::abs(baseDt), std::abs(candidateDt.value));
        const double tolerance = std::max(
            magnitude * Globals::Constant::Program::stepLimiterTolerance,
            std::numeric_limits<double>::epsilon()
        );

        if (difference < -tolerance) {
            return true; // Smaller dt -> limiter fires sooner than current winner
        }

        if (difference > tolerance) {
            return false; // Larger dt -> would delay the step
        }

        if (difference < 0.0) {
            return true; // Still strictly smaller, even if difference is within tolerance
        }

        if (difference > 0.0) {
            return false; // Still strictly larger, even if difference is tiny
        }

        const int basePriority = static_cast<int>(baseEvent.limiter);
        const int candidatePriority = static_cast<int>(limiter);
        return candidatePriority > basePriority;
    }

    void considerCandidate(
        StepEvent &baseEvent,
        const StepLimiter limiter,
        const Quantity &candidateDt,
        const Vector<3> &candidateDisplacement,
        const BoundaryEvent &candidateBoundary
    ) {
        if (!shouldReplaceEvent(baseEvent, limiter, candidateDt)) {
            return;
        }

        baseEvent.limiter = limiter;
        baseEvent.dt = candidateDt;
        baseEvent.displacement = candidateDisplacement;
        baseEvent.boundaryEvent = candidateBoundary;
    }

    void finalizeEvent(StepEvent &event, const Object *world) {
        event.postStep.position = event.preStep.position + event.displacement;
        event.postStep.globalTime = event.preStep.globalTime + event.dt;
        if (event.limiter == StepLimiter::Boundary) {
            event.postStep.medium = event.boundaryEvent.mediumAfter;
        }

        if (event.postStep.medium == nullptr) {
            event.postStep.medium = step_utilities::resolveMediumIfAvailable(world, event.postStep.position);
        }
    }
} // namespace

void applyDiscreteInteraction(std::unique_ptr<Particle> &particle, const Object *world) {
    if (!particle) {
        return;
    }
    const Object *medium = step_utilities::resolveMediumIfAvailable(world, particle->getPosition());

    if (const auto *process = particle->getPendingInteractionProcess()) {
        process->apply(particle, medium);
    }
}

StepEvent determineStepEvent(
    const Particle &particle,
    const Quantity &remainingTime,
    const StepPoint &preStep,
    const Object *world
) {
    if (world == nullptr) {
        throw std::runtime_error("Active world is not available while determining the step event");
    }

    if (!std::isfinite(remainingTime.value) || remainingTime.value <= 0.0) {
        StepEvent zeroStep{};
        zeroStep.preStep = preStep;
        zeroStep.postStep = preStep;
        zeroStep.dt = remainingTime;
        zeroStep.dt.value = 0.0;
        return zeroStep;
    }

    StepEvent event = makeTimeLimitedEvent(particle, remainingTime, preStep);

    auto boundaryDisplacement = event.displacement;
    auto dtBoundary = remainingTime;
    BoundaryEvent boundaryEvent{};
    const bool hitBoundary = preStep.medium != nullptr &&
                             particleBoundaryConditions(
                                 world,
                                 preStep.medium,
                                 preStep.position,
                                 boundaryDisplacement,
                                 dtBoundary,
                                 boundaryEvent
                             );

    const auto speed = particle.getSpeed();
    const bool stationary = speed.value <= 0.0;

    if (hitBoundary) {
        considerCandidate(event, StepLimiter::Boundary, dtBoundary, boundaryDisplacement, boundaryEvent);
    }

    if (!stationary && particle.hasPendingInteractionLength()) {
        const auto interactionDistance = particle.getInteractionLengthRemaining();
        const auto timeToInteraction = interactionDistance / speed;
        const auto interactionDisplacement = displacement(particle, timeToInteraction);
        considerCandidate(event, StepLimiter::Interaction, timeToInteraction, interactionDisplacement, {});
    }

    if (particle.hasDecayClock()) {
        const auto &timeToDecay = particle.getDecayTimeRemaining();
        const auto decayDisplacement = displacement(particle, timeToDecay);
        considerCandidate(event, StepLimiter::Decay, timeToDecay, decayDisplacement, {});
    }

    if (event.dt.value <= 0.0) {
        event.dt.value = 0.0;
    }

    finalizeEvent(event, world);

    return event;
}

void processDiscreteLimiterEvent(std::unique_ptr<Particle> &particle, const StepLimiter limiter, const Object *world) {
    if (!particle) { return; }
    switch (limiter) {
        case StepLimiter::Interaction: {
            applyDiscreteInteraction(particle, world);
            particle->clearInteractionLength();
            break;
        }
        case StepLimiter::Decay: {
            static const auto *decayProcess = discrete_interaction::findInteractionProcess("Spontaneous emission");
            const Object *medium = step_utilities::resolveMediumIfAvailable(world, particle->getPosition());
            if (decayProcess != nullptr) { decayProcess->apply(particle, medium); }
            particle->clearInteractionLength();
            break;
        }
        default:
            break;
    }
}