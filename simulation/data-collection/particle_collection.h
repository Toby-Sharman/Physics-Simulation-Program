//
// Created by Tobias Sharman on 15/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H

#include <string>
#include "particle.h"
#include "box.h"

// Logs the particle's energy to a CSV file if it is inside the given box
void logEnergyIfInside(const Particle& p, const Box& box, const std::string& filename);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H