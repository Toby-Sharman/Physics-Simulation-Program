//
// Physics Simulation Program
// File: particle_continuous_interactions.cpp
// Created by Tobias Sharman on 21/10/2025
//
// Description:
//   - Implementation of particle_continuous_interactions.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/continuous/particle_continuous_interactions.h"

#include "physics/processes/interaction_utilities.h"

Vector<3> displacement(const Particle& particle, const Quantity& dt) {
    const auto& momentum = particle.getMomentum();
    const auto scaledTime = dt * speedOfLight();

    if (particle.isMassless()) {
        return momentum.unitVector() * scaledTime;
    }

    const auto& energy = particle.getEnergy();
    return momentum / energy * scaledTime;
}