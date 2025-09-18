//
// Created by Tobias Sharman on 13/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H

#include "base_database.h"

struct ParticleDB : BaseDB<ParticleDB> {
    static std::string getSymbol(const std::string& particle) {
        return getStringProperty(particle, "symbol");
    }

    static Quantity getRestMass(const std::string& particle) {
        return getQuantity(particle, "rest mass");
    }

    static Quantity getCharge(const std::string& particle) {
        return getQuantity(particle, "charge");
    }

    static Quantity getSpin(const std::string& particle) {
        return getQuantity(particle, "spin");
    }

    static std::string getParticleType(const std::string& particle) {
        return getStringProperty(particle, "particle type");
    }
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_DATABASE_H