////////////////////////////////////////////////////////////////////////////////////////////////////
// File:        main.cpp                                                                          //
// Author:      Tobias Sharman                                                                    //
// Created:     28/08/2025                                                                        //
// Last Modified: 29/08/2025
// Description: Main program file                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

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

#include "field_solver.h"

#include "step.h"

#include <memory>
#include <iostream>

// TODO: Boundary conditions for fields - neglect ferromagnetic effects
// TODO: Functions and variables to camelCase
// TODO: Unit handling for vector operations and printing functions
// TODO: Update Matrix class to handle units too
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

    // Objects
    const auto world = construct<Box>(
        Name("World"),
        Material("vacuum"),
        Size(Vector<3>(Data{100.0,100.0,100.0}, Labels{"L", "W", "H"}, Units{"m", "m", "m"}))
        );
    auto cell = world->addChild<Box>(
        Name("Cell"),
        Material("glass"),
        Position(Vector<3>(Data{1.0,0.0,0.0}, Labels{"x", "y", "z"}, Units{"m", "m", "m"})),
        Size(Vector<3>(Data{5.0,5.0,5.0}, Labels{"L", "W", "H"}, Units{"m", "m", "m"}))
        );
    // Print hierarchy
    world->printHierarchy();

    const auto field = getFieldAtPoint(Vector<3>{{0,0,0}}, world);
    field.print();

    // // Particles
    // auto a = Particle();
    // a.print();
    // // Massive particle
    // auto e = Particle("electron", 0.511, -1, 0.5,{0,0,0,0}, {1e6,1e3,0,1e6}, {0,0,0});  // Create an object of MyClass
    // for(int i=0; i<5; ++i) {
    //     std::cout << e.get_rest_mass() << "\n";
    //     step(e);
    //     std::cout << "t=" << e.get_position()[0]
    //               << " x=" << e.get_position()[1]
    //               << " y=" << e.get_position()[2]
    //               << " z=" << e.get_position()[3] << "\n";
    // }
    //
    // // Photon
    // auto photon = Particle("photon", 0, 0, 1,{0,0,0,0}, {1,0,0,1}, {0,0,0});  // Create an object of MyClass
    // std::cout << "hi" << "\n";
    // photon.print();
    // std::cout << "\nPhoton:\n";
    // for(int i=0; i<5; ++i) {
    //     std::cout << photon.get_rest_mass() << "\n";
    //     step(photon);
    //     std::cout << "t=" << photon.get_position()[0]
    //               << " x=" << photon.get_position()[1]
    //               << " y=" << photon.get_position()[2]
    //               << " z=" << photon.get_position()[3] << "\n";
    // }
}