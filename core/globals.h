//
// Physics Simulation Program
// File: globals.h
// Created by Tobias Sharman on 01/09/2025.
//
// Description:
//   - Contains constants and variables that are:
//         -> Relevant macro-esque details that determine functionality of the entire program
//         -> Maths and physics values that are used in the program so they can be consolidated
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_GLOBALS_H
#define PHYSICS_SIMULATION_PROGRAM_GLOBALS_H

#include <string_view>

// Globals
//
// A global class to hold variables and constants to avoid duplication and/or discrepancy in values used, and give easy
// access to macro-esque constants for easy alteration
class Globals {
    public:
        // Singleton access
        static Globals &instance() {
            static Globals instance;
            return instance;
        };

        // No copies allowed
        Globals(const Globals&) = delete;
        Globals& operator=(const Globals&) = delete;

        // Compile-time constants
        struct Constant {
            // Macro-esque details to alter program functionality
            struct Program{
                static constexpr std::string_view filenamePrefix = "Energies";        // Data output file name prefix
                static constexpr std::string_view outputFolder = "Output";            // Data output folder name (in Build)

                static constexpr double timeStep = 1e-8;                              // Seconds
                // static constexpr double masslessTolerance = 0;                        // Massless tolerance
            };
            // Common maths constants to ensure same value is used program wide
            struct Maths {
                static constexpr double pi = 3.141592653589793238462643383279502884L; // Pi
                static constexpr double deg2rad = pi / 180.0;                         // Degree to radian ratio
                static constexpr double rad2deg = 180.0 / pi;                         // Radian to degree ratio
            };
            // Relevant physics constants, i.e. values aren't variant based on material or particles
            // Physics constants from: CODATA recommended values of the fundamental physical constants: 2022
            struct Physics {
                static constexpr double c = 299792458;                                // Speed of light in m s^-1
                static constexpr double h = 6.62607015e-34;                           // Planck constant
                static constexpr double hbar = h/Maths::pi;                           // Reduced Planck constant
                static constexpr double e = 1.602176634e-19;                          // Elementary charge
                static constexpr double mu0 = 1.25663706127e-6;                       // Vacuum permeability
                static constexpr double u = 1.66053906892e-27;                        // Atomic mass unit in kg
                static constexpr double Da = u;                                       // Dalton
            };
        };

    private:
        ~Globals() = default;
        Globals() = default;
};

#endif //PHYSICS_SIMULATION_PROGRAM_GLOBALS_H