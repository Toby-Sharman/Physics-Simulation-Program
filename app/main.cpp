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
#include "particles/particle_manager.h"
#include "particles/particle_source.h"
#include "simulation/step.h"

// TODO: Think on taking the particle dbs out of memory once there use is done. Is this any good; will need to create new particles from interactions so would have to load up again. Maybe later implement a way to keep only particles that have been loaded up in a shorter kept db and once the big d is used remove it from memory
// TODO: Update Matrix class to handle units too
// TODO: Boundary conditions for fields - neglect ferromagnetic effects
// TODO: Fold particle class into object class???
// TODO: Enforce dimensions on certain parameters
// TODO: Charged particle handling in B/H fields
// TODO: Object overlap - especially child being through parent
// TODO: See what includes I really need/want - especially in files or their dependencies that won't be changed meaningfully

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
        material("glass"), // TODO
        size(Vector<3>({3.0, 3.0, 3.0}, "mm"))
        );
    const auto collection = world->addChild<Box>( // TODO: Change collection from a box to if particle x > 12.5mm
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
     ParticleManager manager;
     ParticleSource source;

     // Generate 100 photons around a mean position/momentum
     const auto photonBatch = source.generateParticles(
         "photon",
         1000000,
         Vector<4>({0,0,0,0}, "m"),
         Vector<4>({1,1,0,0}, "kg m s^-1"),
         Vector<3>({0,0,0})
     );

     manager.addParticles(photonBatch);

     // Simulation loop
     for (int i = 0; i < 50; ++i) {
         if (manager.empty()) {
             break;
         }
         stepAll(collection, manager.getParticles()); // all particles automatically stepped
     }

    return 0;
}