//
// Created by Tobias Sharman on 15/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H

#include "particle.h"

#include "globals.h"

#include "box.h"

#include <string>

// Logs the particle's energy to a CSV file if it is inside the given box
void logEnergyIfInside(const Particle &p, const Box &box,
                       const std::string &baseFolder = Globals::Constant::OUTPUT_FOLDER,
                       const std::string &baseFilename = Globals::Constant::FILENAME_PREFIX);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H