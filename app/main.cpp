//
// Created by Tobias Sharman on 28/08/2025.
//

#include "config.h"

#include "vector.h"
#include "matrix.h"
#include "rotation_helpers.h"

#include "globals.h"
#include "material_database.h"
#include "particle_database.h"

#include "object_tags.h"

#include "object.h"
#include "box.h"
#include "sphere.h" // FIXME
#include "point.h"  // FIXME

#include "particle.h"
#include "particle_manager.h"
#include "particle_source.h"

#include "field_solver.h"

#include "step.h"

#include <memory>
#include <iostream>

// TODO: Unit handling
// TODO: Add some description to headers
// TODO: Boundary conditions for fields - neglect ferromagnetic effects
// TODO: Unit handling for vector operations and printing functions
// TODO: Update Matrix class to handle units too
// TODO: Fold particle class into object class???
// TODO: Charged particle handling in B/H fields
// TODO: Object overlap - especially child being through parent
// TODO: See what includes I really need/want - especially in files or their dependencies that won't be changed meaningfully

int main() {
    MaterialDB::loadFromBinary(MATERIAL_DATABASE_DB_PATH);
    ParticleDB::loadFromBinary(PARTICLE_DATABASE_DB_PATH);

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
    //     Data{0.0,0.0,20.0},
    //     Labels{"x", "y", "z"},
    //     Units{"T", "T", "T"}), world);
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