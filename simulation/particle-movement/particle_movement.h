//
// Created by Tobias Sharman on 17/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H

#include "globals.h"
#include "particle.h"
#include "globals.h"

// Moves a single particle forward by dt
void moveParticle(Particle& p, double dt = Globals::Constant::Program::timeStep);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H