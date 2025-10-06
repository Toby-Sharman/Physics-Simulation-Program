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
#include "objects/utilities/object_tags.h"
#include "particles/particle.h"
#include "particles/particle_manager.h"
#include "particles/particle_source.h"
#include "simulation/step.h"

// TODO: Think on taking the particle dbs out of memory once there use is done. Is this any good; will need to create new particles from interactions so would have to load up again. Maybe later implement a way to keep only particles that have been loaded up in a shorter kept db and once the big d is used remove it from memory
// TODO: Update Matrix class to handle units too
// TODO: Boundary conditions for fields - neglect ferromagnetic effects
// TODO: Fold particle class into object class???
// TODO: Charged particle handling in B/H fields
// TODO: Object overlap - especially child being through parent
// TODO: See what includes I really need/want - especially in files or their dependencies that won't be changed meaningfully

int main() {

    // try {
    //
    //     auto [scale1, dim1] = parseUnit("m^3");
    //     std::cout << "m^3 → scale=" << scale1
    //               << " dim=" << dim1.toString() << "\n";
    //
    //     auto [scale2, dim2] = parseUnit("cm");
    //     std::cout << "cm → scale=" << scale2
    //               << " dim=" << dim2.toString() << "\n";
    //
    //     auto [scale3, dim3] = parseUnit("m⁻³");
    //     std::cout << "m⁻³ → scale=" << scale3
    //               << " dim=" << dim3.toString() << "\n";
    //
    //     auto [scale4, dim4] = parseUnit("kg-1");
    //     std::cout << "kg-1 → scale=" << scale4
    //               << " dim=" << dim4.toString() << "\n";
    //
    //
    //     UnitInfo u1 = parseUnits("kg m^2 / s^2");
    //     std::cout << "kg m^2 / s^2 → scale=" << u1.scale
    //               << " dim=" << u1.dimension.toString() << "\n";
    //
    //     UnitInfo u2 = parseUnits("cm·s^-1");
    //     std::cout << "cm·s^-1 → scale=" << u2.scale
    //               << " dim=" << u2.dimension.toString() << "\n";
    //
    //     UnitInfo u3 = parseUnits("N m");
    //     std::cout << "N m → scale=" << u3.scale
    //               << " dim=" << u3.dimension.toString() << "\n";
    //
    // } catch (const std::exception& ex) {
    //     std::cerr << "Error: " << ex.what() << "\n";
    //     return 1;
    // }
    //
    // return 0;

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
        Name("World"),
        Material("vacuum"),
        Size(Vector<3>({100.0, 50.0, 50.0}, "mm"))
        );
    const auto cell = world->addChild<Box>(
        Name("Cell"),
        Material("glass"),
        Size(Vector<3>({25.0, 15.0, 15.0}, "mm"))
        );
    const auto vapourCell = cell->addChild<Box>(
        Name("Vapour Cell"),
        Material("glass"), // TODO
        Size(Vector<3>({3.0,3.0,3.0}, "mm"))
        );
    const auto collection = world->addChild<Box>( // TODO: Change collection from a box to if particle x > 12.5mm
        Name("Collection"),
        Material("vacuum"),
        Position(Vector<3>({12.5+37.5/2,0.0,0.0}, "m")),
        Size(Vector<3>({37.5,50.0,50.0}, "m"))
        );
    // Print hierarchy
    world->printHierarchy();

    // const auto field = getFieldAtPoint(Vector<3>(
    //     {0.0,0.0,20.0},
    //     "T"), world);
    // field.print();
    //
    // std::cout << "\n\n\n" << std::endl;

    // Particles
    ParticleManager manager;

    manager.addParticle(
        "photon",
        Vector<4>({0,0,0,0}),
        Vector<4>({1,1,0,0}),
        Vector<3>({0,0,0})
    );

    manager.addParticle(
        "electron",
        Vector<4>({0,1,0,0}),
        Vector<4>({1,1,0,0}),
        Vector<3>({0,0,1})
    );

    ParticleSource source;

    // Generate 100 photons around a mean position/momentum
    const auto photonBatch = source.generateParticles(
        "photon",
        100,
        Vector<4>({0,0,0,0}),
        Vector<4>({1,1,0,0}),
        Vector<3>({0,0,0})
    );

    manager.addParticles(photonBatch);

    // Simulation loop
    for (int i = 0; i < 50; ++i) {
        if (manager.empty()) break;
        stepAll(collection, manager.getParticles()); // all particles automatically stepped
    }

    return 0;
}