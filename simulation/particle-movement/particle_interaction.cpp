//
// Physics Simulation program
// File: particle_interaction.cpp
// Created by Tobias Sharman on 13/10/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include <cmath>
#include <random>
#include <string>

#include "simulation/particle-movement/particle_interaction.h"
#include "core/globals.h"
#include "databases/material-data/material_database.h"
#include "objects/object.h"
#include "objects/object-types/box.h"
#include "particles/particle.h"

double interactionProbability(const Particle& particle, const Vector<3> &displacement,  const std::shared_ptr<Box>& world) {
    const auto material = world->findObjectContaining(*reinterpret_cast<Vector<3> const*>(&particle.getPosition()[1]))->getMaterial();

    const auto crossSection = Quantity(1e-20, "m^2"); // Placeholder FIXME

    const auto opticalDepth = materialDatabase.getNumberDensity(material) * crossSection * displacement.length();

    return 1 - std::pow(Globals::Constant::Maths::e, -opticalDepth.value);
}

void interaction(const Particle& particle, const Vector<3> &displacement,  const std::shared_ptr<Box>& world) {
    std::random_device rd;
    std::ranlux48 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    bool selectionRules = true; // Placeholder

    if (particle.getType() == "photon") {
        if (dist(gen) >= interactionProbability(particle, displacement, world)) {
        }
        const auto matterParticle = std::make_unique<Particle>("gas", // TODO: Take matter particle name from object inside - move bit from interaction probability for material into interaction
            particle.getPosition(), // TODO: Decide on where to create the particle
            Vector<4>({1,1,0,0}, "kg m s^-1"), // TODO make random
            Vector<4>({0,0,0, 0}) // TODO make random - need basis in larmor
            );
        if (selectionRules) {
            matterParticle->setMomentum(matterParticle->getMomentum() + particle.getMomentum());
            // Delete photon
            // Add matter particle to particle manager
            // Move particle so time field of 4 position line up,- i.e. position[0]/timeStep == 0
        }
    } else if (particle.getType() == "gas") { // Maybe != photon instead
        // If matter particle lifetime - corresponds to how long till deexcitation - probably need be a random process too.
        //      Create photon and attach to particle manager
        //      Delete matter particle
        //      move particle so time field of 4 position line up,- i.e. position[0]/timeStep == 0
        // Else
        //      Larmor precession and move particle
    }
}