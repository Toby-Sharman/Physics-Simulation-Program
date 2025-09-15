//
// Created by Tobias Sharman on 13/09/2025.
//

#ifndef SIMULATION_PROGRAM_PARTICLE_DATABASE_H
#define SIMULATION_PROGRAM_PARTICLE_DATABASE_H

#include "base_database.h"

class ParticleDB : public BaseDB<ParticleDB> {
public:
    static double getCharge(const std::string& particle) {
        return getProperty(particle, "charge");
    }

    static double getMass(const std::string& particle) {
        return getProperty(particle, "mass");
    }
};

#endif //SIMULATION_PROGRAM_PARTICLE_DATABASE_H