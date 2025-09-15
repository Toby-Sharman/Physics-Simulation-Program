////////////////////////////////////////////////////////////////////////////////////////////////////
// File:        main.cpp                                                                          //
// Author:      Tobias Sharman                                                                    //
// Created:     28/08/2025                                                                        //
// Last Modified: 29/08/2025
// Description: Main program file                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "output_file_name.h"

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

#include "field_solver.h"

#include "step.h"

#include <memory>
#include <iostream>

// TODO: Boundary conditions for fields - neglect ferromagnetic effects
// TODO: Functions and variables to camelCase
// TODO: Unit handling for vector operations and printing functions
// TODO: Update Matrix class to handle units too
// TODO: New Vector creation style
// TODO: For particle definition shift some stuff to be defined from the particle db
// TODO: Fold particle class into object class???
// TODO: Set default values for the particles created vector labels when giving values
// TODO: Charged particle handling in B/H fields
// TODO: JSON files for units, globals? - Probably not though
// TODO: Object overlap - especially child being through parent
// TODO: See what includes I really need/want - especially in files or their dependencies that won't be changed meaningfully

int main() {
    MaterialDB::loadFromBinary(MATERIAL_DATABASE_DB_PATH);
    ParticleDB::loadFromBinary(PARTICLE_DATABASE_DB_PATH);
    const std::string filename = generateSequentialFilename();

    // Objects
    const auto world = construct<Box>(
        Name("World"),
        Material("vacuum"),
        Size(Vector<3>(Data{100.0,100.0,20.0}, Labels{"L", "W", "H"}, Units{"m", "m", "m"}))
        );
    auto cell = world->addChild<Box>(
        Name("Cell"),
        Material("glass"),
        Position(Vector<3>(Data{0.0,0.0,0.0}, Labels{"x", "y", "z"}, Units{"m", "m", "m"})),
        Size(Vector<3>(Data{4.0,4.0,4.0}, Labels{"L", "W", "H"}, Units{"m", "m", "m"}))
        );
    auto collection = world->addChild<Box>(
        Name("Collection"),
        Material("vacuum"),
        Position(Vector<3>(Data{26.0,0.0,0.0}, Labels{"x", "y", "z"}, Units{"m", "m", "m"})),
        Size(Vector<3>(Data{48.0,50.0,20.0}, Labels{"L", "W", "H"}, Units{"m", "m", "m"}))
        );
    // Print hierarchy
    world->printHierarchy();

    const auto field = getFieldAtPoint(Vector<3>(
        Data{0.0,0.0,20.0},
        Labels{"x", "y", "z"},
        Units{"T", "T", "T"}), world);
    field.print();

    std::cout << "\n\n\n" << std::endl;

    // Particles
    static constexpr double mass = 1.0;
    auto custom = Particle("electron", mass, -1, 0.5,Vector<4>({0.0, 0.0, 0.0, 0.0}), Vector<4>({std::sqrt( mass*mass + 0.1005*0.1005 ),0.1005,0,0}), Vector<3>({0,0,0}));  // Create an object of MyClass
    for(int i=0; i<5; ++i) {
        step(collection, filename, custom);
        std::cout << "t=" << custom.getPosition()[0]
                  << " x=" << custom.getPosition()[1]
                  << " y=" << custom.getPosition()[2]
                  << " z=" << custom.getPosition()[3] << "\n";
    }

    std::cout << "\n\n\n" << std::endl;

    // Photon
    auto photon = Particle("photon", 0, 0, 1,Vector<4>({0,0,0,0}), Vector<4>({1,1,0,0}), Vector<3>({0,0,0}));  // Create an object of MyClass
    for(int i=0; i<25; ++i) {
        step(collection, filename,photon);
        std::cout << "t=" << photon.getPosition()[0]
                  << " x=" << photon.getPosition()[1]
                  << " y=" << photon.getPosition()[2]
                  << " z=" << photon.getPosition()[3] << "\n";
    }
}