//
// Physics Simulation Program
// File: main.cpp
// Created by Tobias Sharman on 28/08/2025
//
// Description:
//   - Main entry point for the program
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

// Reminder: what includes am I missing? Can I mark any functions nodiscard, noexcept, constexpr, or static?

#include <iomanip>
#include <iostream>
#include <memory>

#include "core/maths/vector.h"
#include "objects/box.h"
#include "objects/object.h"
#include "particles/particle.h"
#include "particles/particle_source.h"
#include "simulation/step.h"

int main() {

    // Objects
    /* Not to scale
     * + -------------------------------------------------- +
     * | World                                 |            |
     * |              + ---------------------- +            |
     * |              | Cell                   |            |
     * |              |     + ---------- +     | Collection |
     * |       0      |     |   Vapour   |     |            |
     * |   Particle   |     + ---------- +     |   Region   |
     * |    Source    |                        |            |
     * |              + ---------------------- +            |
     * |                                       |            |
     * + -------------------------------------------------- +
     *
     * World 100mm x 50mm x 50mm
     * Entire cell 25mm x 15mm x 15mm
     * Vapour cell 3mm x 3mm x 3mm
     */

    const auto world = construct<Box>(
        name("World"),
        material("vacuum"),
        size(Vector<3>({100.0, 50.0, 50.0}, "mm"))
        );
    const auto cell = world->addChild<Box>(
        name("Cell"),
        material("glass"),
        size(Vector<3>({25.0, 15.0, 15.0}, "mm"))
        );
    const auto vapourCell = cell->addChild<Box>(
        name("Vapour Cell"),
        material("gas"),
        size(Vector<3>({3.0, 3.0, 3.0}, "mm"))
        );
    const auto collection = world->addChild<Box>(
        name("Collection"),
        material("vacuum"),
        position(Vector<3>({12.5+37.5/2, 0.0, 0.0}, "m")),
        size(Vector<3>({37.5, 50.0, 50.0}, "m"))
        );
    // Print hierarchy
    world->printHierarchy();

    // const auto field = getFieldAtPoint(Vector<3>(
    //     {0.0,0.0,20.0},
    //     "T"), world);
    // field.print();

    // Particles
     ParticleSource source;

     // Generate 100 photons around a mean position/momentum
     source.generateParticles(
         "photon",
         1000000,
         Vector<4>({-(5+12.5+37.5/2),0,0,0}, "mm"),
         Vector<4>({1,1,0,0}, "kg m s^-1"),
         Vector<4>({1,0,0, 1}) // Right hand circular polarised TODO: Think on units
     );

     // Simulation loop
    while (!particleManager.empty()) { // TODO: NEED to add a check for particles are outside world to delete them
        stepAll(particleManager.getParticles(), world, collection); // all particles automatically stepped
    }

    return 0;
}