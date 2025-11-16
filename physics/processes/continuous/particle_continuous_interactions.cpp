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
    if (!std::isfinite(dt.value) || dt.value == 0.0) {
        return {};
    }

    const auto& momentum = particle.getMomentum();
    const auto momentumMagnitude = momentum.length();
    if (momentumMagnitude.value <= 0.0) {
        return {};
    }

    const auto c = speedOfLight();
    if (particle.isMassless()) {
        return momentum / momentumMagnitude * (c * dt);
    }

    const auto energy = particle.getEnergy();
    if (energy.value == 0.0) {
        return {};
    }

    const auto velocity = momentum / energy * (c * c);
    return velocity * dt;
}