//
// Created by Tobias Sharman on 17/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H

#include "particle.h"
#include "core/maths/utilities/quantity.h"
#include "objects/object-types/box.h"

Vector<4> displacement(const Particle& particle, Quantity dt);

// Moves a single particle forward by dt
void moveParticle(Particle& particle, const Object*& world, const Quantity& dt = quantityTable().at("time step"));

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MOVEMENT_H