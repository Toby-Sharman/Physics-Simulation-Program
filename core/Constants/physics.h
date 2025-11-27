//
// Physics-Simulation-program
// File: physics.h
// Created by Tobias Sharman on 27/11/2025
//
// Description:
//   - Physics related constants
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PHYSICS_H
#define PHYSICS_SIMULATION_PROGRAM_PHYSICS_H

#include <numbers>

#include "constants/maths.h"

namespace constants::physics {
    inline constexpr double c = 299792458;                         // Speed of light in m s^-1
    inline constexpr double h = 6.62607015e-34;                    // Planck constant
    inline constexpr double hbar = h / (2 * constants::math::pi);  // Reduced Planck constant
    inline constexpr double e = 1.602176634e-19;                   // Elementary charge
    inline constexpr double mu0 = 1.25663706127e-6;                // Vacuum permeability
    inline constexpr double u = 1.66053906892e-27;                 // Atomic mass unit in kg
    inline constexpr double Da = u;                                // Dalton
    inline constexpr double k_b = 1.380649e-23;                    // Boltzmann constant
} // namespace constants::physics

#endif //PHYSICS_SIMULATION_PROGRAM_PHYSICS_H
