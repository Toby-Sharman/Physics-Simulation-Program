//
// Physics Simulation Program
// File: decay_utilities.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Helpers for decay processes
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_DECAY_UTILITIES_H
#define PHYSICS_SIMULATION_PROGRAM_DECAY_UTILITIES_H

#include "core/quantities/quantity.h"
#include "particles/particle.h"

namespace discrete_interaction {
    // Samples a random decay time using the particle's intrinsic lifetime
    [[nodiscard]] Quantity sampleDecayTime(const Particle &particle);
} // namespace discrete_interaction

#endif //PHYSICS_SIMULATION_PROGRAM_DECAY_UTILITIES_H