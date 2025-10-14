//
// Physics Simulation program
// File: particle_interaction.cpp
// Created by Tobias Sharman on 13/10/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include <cmath>
#include <string>

#include "simulation/particle-movement/particle_interaction.h"
#include "core/globals.h"
#include "databases/material-data/material_database.h"
#include "objects/object.h"
#include "particles/particle.h"

double interactionProbability(const Particle& particle, const Vector<3> &displacement) {
    const auto material = Object::findObjectContainingPoint(*reinterpret_cast<Vector<3> const*>(&particle.getPosition()[1]))->getMaterial();

    const auto crossSection = Quantity(10e-20, "m^2"); // Placeholder FIXME

    const auto opticalDepth = materialDatabase.getNumberDensity(material) * crossSection * displacement.length();

    return 1 - std::pow(Globals::Constant::Maths::e, -opticalDepth.value);
}

void interaction(const Particle& particle, Vector<3> displacement) {
    if (particle.getParticleName() == "photon") {
        if (!interacts) {
        }
        if (!selection rules) {
        }

    } else if (particle.getParticleName() == "gas") {

    }
}