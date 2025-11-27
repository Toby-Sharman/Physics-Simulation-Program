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

#include <optional>
#include <string>

#include "config/path_config.h"
#include "databases/base_database.h"
#include "particles/particle-types/particle_type.h"

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

    [[nodiscard]] Quantity getCharge(const std::string& particle) const {
        return getQuantityProperty(particle, "charge");
    }

    [[nodiscard]] Quantity getSpin(const std::string& particle) const {
        return getQuantityProperty(particle, "spin");
    }

    [[nodiscard]] ParticleType getParticleType(const std::string& particle) const {
        try {
            const auto type = getStringProperty(particle, "particle type");
            if (type == "photon") {
                return ParticleType::Photon;
            }
            if (type == "atom") {
                return ParticleType::Atom;
            }
        } catch (const std::runtime_error&) {
            // Property missing; fall through to generic classification
        }
        return ParticleType::Generic;
    }

    [[nodiscard]] Quantity getLifetime(const std::string& particle) const {
        return getQuantityProperty(particle, "lifetime");
    }

    [[nodiscard]] std::optional<double> getNuclearSpin(const std::string& particle) const {
        try {
            return getNumericProperty(particle, "nuclearSpin");
        } catch (const std::runtime_error&) {
            return std::nullopt;
        }
    }
};

// Create a reusable instance of the particle database
inline ParticleDatabase g_particleDatabase{std::string(config::paths::particleDatabasePath)};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H
