//
// Physics Simulation Program
// File: step_events.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Step limiter types and helpers for discrete/continuous event selection
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_STEP_EVENTS_H
#define PHYSICS_SIMULATION_PROGRAM_STEP_EVENTS_H

#include <memory>

#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"
#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_process.h"
#include "simulation/geometry/boundary/boundary_interactions.h"

using SpawnQueue = discrete_interaction::SpawnQueue;

// Step limiter ordering used when candidate events compete for the next step; higher values win for ties on dt
enum class StepLimiter : int {
    Time = 0,
    Boundary = 1,
    Decay = 2,
    Interaction = 3
};

// Snapshot of the particle state before and after a proposed step
struct StepPoint {
    Vector<3> position = Vector<3>();                          // Cartesian position at the snapshot
    Quantity globalTime = Quantity::dimensionless(0.0);   // Absolute simulation time
    const Object* medium = nullptr;                            // Medium that currently contains the particle
};

// Aggregate of the resolved limiter, duration, displacement, and boundary metadata for a step
struct StepEvent {
    StepLimiter limiter = StepLimiter::Time;              // Mechanism that capped this step
    Quantity dt = Quantity::dimensionless(0.0);      // Duration that will be advanced
    Vector<3> displacement = Vector<3>();                 // World displacement over dt
    BoundaryEvent boundaryEvent{};                        // Populated when limiter == Boundary
    StepPoint preStep{};                                  // State at the beginning of the step
    StepPoint postStep{};                                 // Expected state at the end of the step
};

// Apply a deferred discrete interaction process (if any) using the medium resolved from the world
void applyDiscreteInteraction(std::unique_ptr<Particle>& particle, const Object* world, SpawnQueue& spawned);

// Evaluate all candidate limiters and return the next step event capped by the highest-priority limiter
StepEvent determineStepEvent(
    const Particle& particle,
    const Quantity& remainingTime,
    const StepPoint& preStep,
    const Object* world
    );

// Execute any secondary effects once a discrete limiter actually fires (decay/interaction bookkeeping)
void processDiscreteLimiterEvent(
    std::unique_ptr<Particle>& particle,
    StepLimiter limiter,
    const Object* world,
    SpawnQueue& spawned);

#endif //PHYSICS_SIMULATION_PROGRAM_STEP_EVENTS_H
