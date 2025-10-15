//
// Physics Simulation program
// File: particle_interaction.h
// Created by Tobias Sharman on 13/10/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_INTERACTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_INTERACTION_H

#include "objects/object-types/box.h"
#include "particles/particle.h"

double interactionProbability(const Particle& particle, const Vector<3> &displacement,  const std::shared_ptr<Box>& world);

void interaction(const Particle& particle, const Vector<3> &displacement, const std::shared_ptr<Box>& world);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_INTERACTION_H