//
// Physics Simulation Program
// File: program_config.h
// Created by Tobias Sharman on 27/11/2025.
//
// Description:
//   - Program constants configuration
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PROGRAM_CONFIG_H
#define PHYSICS_SIMULATION_PROGRAM_PROGRAM_CONFIG_H

#include <cstddef>

namespace config::program {
    inline constexpr std::size_t maxWorkerThreads = 0;           // 0 -> auto-detect (hardware_concurrency) else value = actual thread count set

    inline constexpr double timeStep = 1e-15;                    // Increment of time in each step of the program (in seconds)
    inline constexpr double masslessTolerance = 0;               // Tolerance for treating particles as massless (in kg)
    inline constexpr double geometryTolerance = 1e-10;           // Relative/absolute scale for geometry comparisons
    inline constexpr double lorentzGammaLimit = 1e6;             // Maximum allowed Lorentz factor before clamping
    inline constexpr double timeSynchronisationTolerance = 1e-9; // Relative tolerance for Particle::synchroniseTime()
    inline constexpr double hyperfineSelectionTolerance = 1e-9;  // Relative tolerance for Atom hyperfine level matching
    inline constexpr double stepLimiterTolerance = 1e-9;         // Relative tolerance when comparing competing step events
} // namespace config::program

#endif //PHYSICS_SIMULATION_PROGRAM_PROGRAM_CONFIG_H
