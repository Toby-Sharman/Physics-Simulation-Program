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

// Reminder: what includes am I missing? Can I mark any functions nodiscard, noexcept, constexpr?
// TODO:
//      Improve compile time

#include <chrono>
#include <iomanip>
#include <iostream>

#include "core/linear-algebra/matrix.h"
#include "core/linear-algebra/vector.h"
#include "core/random/random_manager.h"
#include "objects/object.h"
#include "objects/object_manager.h"
#include "objects/object-types/box.h"
#include "particles/particle_manager.h"
#include "particles/particle_source.h"
#include "simulation/stepping/step_manager.h"

int main() {
    constexpr std::uint64_t masterSeed = 0x123456789ABCDEFull;
    random_manager::setMasterSeed(masterSeed);
    random_manager::setStreamSeed(random_manager::Stream::DiscreteInteractions, masterSeed + 1);
    random_manager::setStreamSeed(random_manager::Stream::ThermalVelocities, masterSeed + 2);
    random_manager::setStreamSeed(random_manager::Stream::SourceSampling, masterSeed + 3);
    random_manager::resetCachedEngines();

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

    auto* world = g_objectManager.createWorld<Box>(
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
        position(Vector<3>({25.0/2 + 37.5/2, 0.0, 0.0}, "mm")),
        size(Vector<3>({37.5, 50.0, 50.0}, "mm"))
        );
    // Print hierarchy
    world->printHierarchy();

    // const auto field = getFieldAtPoint(Vector<3>(
    //     {0.0,0.0,20.0},
    //     "T"), world);
    // field.print();

    // Particles
    ParticleSource source;

    source.generateParticles(
        "photon",
        10000,
        Quantity(0.0, "s"),
        Vector<3>({-(25.0/2 + 10),0,0}, "mm"),
        Quantity(1.0, "J"),
        Vector<3>({1,0,0}, "kg m s^-1"),
        Vector<4>({1.0, 0.0, 0.0, 1.0}) // Right hand circular polarised TODO: Think on units
        );

    const auto start = std::chrono::steady_clock::now();

    // Simulation loop
    while (!g_particleManager.empty()) {
        stepAll(collection, Quantity(1e-13, "s")); // all particles automatically stepped
    }

    const auto end = std::chrono::steady_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << "Elapsed time: " << duration.count() << " seconds\n";

    return 0;
}