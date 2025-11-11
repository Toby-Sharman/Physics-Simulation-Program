//
// Physics Simulation program
// File: distributions.h
// Created by Tobias Sharman on 15/10/2025
//
// Description:
//   - Contains functions based on attaining data from distributions
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_DISTRIBUTIONS_H
#define PHYSICS_SIMULATION_PROGRAM_DISTRIBUTIONS_H

#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"

// Velocity sampler from temperature
//
// Gives a random velocity for a particle based on the temperature, according to Maxwell-Boltzmann statistics based
// about a normal distribution
//
// Notes on input:
//   - temperature should be taken from the object attribute
//   - particleMass should be taken from the particle properties not the particle table for consistency with custom
//     units
//
// Notes on output:
//   - Generate velocity not a 4-momentum so it is more general
//
// Parameters:
//   - temperature - a reference to a Quantity (should be a temperature)
//   - particleMass - a reference to a Quantity (should be a mass)
//
// Returns:
//   - Vector<3> - A 3-velocity vector
//
// Example usage: TODO
[[nodiscard]] Vector<3> sampleThermalVelocity(const Quantity& temperature, const Quantity& particleMass);


// Isotropic direction sampler
//
// Generates a random unit vector uniformly distributed over the surface of a sphere
//
// Notes on algorithms:
//   - Draws cos(Θ) uniformly in [-1, 1] and φ uniformly in [Θ, 2π) to avoid pole clustering
//   - Theta is measured from +z and φ from +x in the x-y plane; sampling cos(Θ) directly ensures the polar angle
//     density matches the surface area element (sin(Θ) dΘ dφ)
//   - Phi (azimuth) is sampled uniformly over a full revolution [0, 2π) so every rotation around +z is equally likely
//
// Notes on output:
//   - Returns a Vector<3> whose components are all dimensionless and whose magnitude is 1 (within floating precision)
//
// Returns:
//   - Vector<3> - A unit direction vector
//
// Example usage: TODO
[[nodiscard]] Vector<3> sampleIsotropicDirection();

#endif //PHYSICS_SIMULATION_PROGRAM_DISTRIBUTIONS_H