//
// Physics-Simulation-program
// File: maths.h
// Created by Tobias Sharman on 27/11/2025
//
// Description:
//   - Maths related constants
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_MATHS_H
#define PHYSICS_SIMULATION_PROGRAM_MATHS_H

#include <numbers>

namespace constants::math {
    inline constexpr double e = std::numbers::e;               // Euler's number
    inline constexpr double pi = std::numbers::pi;             // Pi
    inline constexpr double deg2rad = pi / 180.0;              // Degree to radian ratio
    inline constexpr double rad2deg = 180.0 / pi;              // Radian to degree ratio
} // namespace constants::math

#endif //PHYSICS_SIMULATION_PROGRAM_MATHS_H
