//
// Physics Simulation Program
// File: particle_continuous_interactions.h
// Created by Tobias Sharman on 21/10/2025
//
// Description:
//   - Functions for types of continuous/integral motion
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_CONTINUOUS_INTERACTIONS_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_CONTINUOUS_INTERACTIONS_H

#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"
#include "particles/particle.h"

// Displacement calculator
//
// Compute spatial displacement for a particle over a time dt; with simple relativistic considerations
//
// Notes on algorithms:
//   - Has different consideration for massless and effectively massless particles
//
// Returns:
//   - Vector<3> - A displacement vector
//
// Example usage: TODO
Vector<3> displacement(const Particle& particle, const Quantity& dt);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_CONTINUOUS_INTERACTIONS_H