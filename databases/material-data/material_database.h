//
// Physics Simulation Program
// File: material_database.h
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Child class to add specific access methods for database type
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_MATERIAL_DATABASE_H
#define PHYSICS_SIMULATION_PROGRAM_MATERIAL_DATABASE_H

#include "config/config.h"
#include "databases/base_database.h"

// MaterialDatabase
//
// Child of BaseDatabase adding specific access methods
struct MaterialDatabase final : BaseDatabase {
    using BaseDatabase::BaseDatabase; // Inherit constructors

    [[nodiscard]] double getRelativePermeability(const std::string& material) const {
        return getNumericProperty(material, "relativePermeability");
    }
};

// Create a reusable instance of the material database
inline MaterialDatabase materialDatabase{MATERIAL_DATABASE_PATH};

#endif //PHYSICS_SIMULATION_PROGRAM_MATERIAL_DATABASE_H
