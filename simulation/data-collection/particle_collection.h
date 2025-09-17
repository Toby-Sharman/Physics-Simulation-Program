//
// Created by Tobias Sharman on 15/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H

#include "particle.h"

#include "globals.h"

#include "box.h"

#include <string>
#include <memory>

// Logs the particle's energy if inside the box and deletes it if condition met
void logEnergyIfInside(std::unique_ptr<Particle>& p,
                       const Box& box,
                       const std::string &baseFolder = Globals::Constant::OUTPUT_FOLDER,
                       const std::string &baseFilename = Globals::Constant::FILENAME_PREFIX);


#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H