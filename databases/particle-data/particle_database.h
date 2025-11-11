//
// Physics Simulation Program
// File: particle_database.h
// Created by Tobias Sharman on 13/09/2025
//
// Description:
//   - Child class to add specific access methods for database type
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H

#include <string>

#include "config/config.h"
#include "databases/base_database.h"

// ParticleDatabase
//
// Child of BaseDatabase adding specific access methods
struct ParticleDatabase final : BaseDatabase {
    using BaseDatabase::BaseDatabase; // Inherit constructors

    [[nodiscard]] std::string getSymbol(const std::string& particle) const {
        return getStringProperty(particle, "symbol");
    }

    [[nodiscard]] Quantity getRestMass(const std::string& particle) const {
        return getQuantityProperty(particle, "rest mass");
    }

    [[nodiscard]] double getCharge(const std::string& particle) const {
        return getNumericProperty(particle, "charge");
    }

    [[nodiscard]] double getSpin(const std::string& particle) const {
        return getNumericProperty(particle, "spin");
    }

    [[nodiscard]] std::string getParticleType(const std::string& particle) const {
        return getStringProperty(particle, "particle type");
    }

    [[nodiscard]] Quantity getLifetime(const std::string& particle) const {
        return getQuantityProperty(particle, "lifetime");
    }
};

// Create a reusable instance of the particle database
inline ParticleDatabase g_particleDatabase{PARTICLE_DATABASE_PATH};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H
