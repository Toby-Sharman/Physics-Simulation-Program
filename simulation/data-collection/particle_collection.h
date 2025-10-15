//
// Created by Tobias Sharman on 15/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H

#include "particle.h"

#include "globals.h"

#include "objects/object-types/box.h"

#include <memory>
#include <string_view>

// Logs the particle's energy if inside the box and deletes it if condition met
void logEnergyIfInside(std::unique_ptr<Particle>& p,
                       const Box& box,
                       const std::string_view &baseFolder = Globals::Constant::Program::outputFolder,
                       const std::string_view &baseFilename = Globals::Constant::Program::filenamePrefix);


#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H