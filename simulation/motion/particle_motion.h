//
// Physics Simulation Program
// File: particle_motion.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Helpers for updating particle under continuous/integral motion
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOTION_H

#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"
#include "particles/particle.h"

// Applies continuous/integral motion to the particle TODO: Better name
void moveParticle(Particle& particle, const Quantity& dt, const Vector<3>& displacement);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOTION_H