//
// Created by Tobias Sharman on 17/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H

#include "globals.h"
#include "particle.h"

Vector<4> displacement(const Particle& particle, Quantity dt);

// Moves a single particle forward by dt
void moveParticle(Particle& particle, const Quantity& dt = quantityTable().at("time step"));

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H