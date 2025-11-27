//
// Physics Simulation Program
// File: path_config.h
// Created by Tobias Sharman on 27/11/2025.
//
// Description:
//   - Program paths configuration (build/output/database locations and naming)
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PATH_CONFIG_H
#define PHYSICS_SIMULATION_PROGRAM_PATH_CONFIG_H

#include <string_view>

namespace config::paths {
    // Paths may be absolute or relative to this config directory.
    inline constexpr std::string_view buildDirectory = "Build";
    inline constexpr std::string_view outputDirectory = "Output";
    inline constexpr std::string_view databaseDirectory = "Databases";

    inline constexpr std::string_view materialDatabasePath = "Databases/material_database.bin";
    inline constexpr std::string_view particleDatabasePath = "Databases/particle_database.bin";

    inline constexpr std::string_view filenamePrefix = "Energies"; // Data output file name prefix
} // namespace config::paths

#endif //PHYSICS_SIMULATION_PROGRAM_PATH_CONFIG_H
