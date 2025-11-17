//
// Physics Simulation Program
// File: particle_type.h
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Defines the different particle types used throughout the simulation; used for checks for particle types for
//     non-user construction
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPE_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPE_H

enum class ParticleType {
    Generic = 0,
    Photon,
    Atom
};

#endif // PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPE_H