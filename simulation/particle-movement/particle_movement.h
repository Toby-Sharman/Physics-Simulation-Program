//
// Created by Tobias Sharman on 17/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H

#include "globals.h"
#include "particle.h"
#include "box.h"

Vector<4> displacement(const Particle& particle, Quantity dt);

// Moves a single particle forward by dt
void moveParticle(Particle& particle, const std::shared_ptr<Box>& world, const Quantity& dt = quantityTable().at("time step"));

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H