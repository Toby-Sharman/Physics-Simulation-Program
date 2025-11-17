//
// Physics Simulation Program
// File: particle_collection.h
// Created by Tobias Sharman on 15/09/2025
//
// Description:
//   - Describe logging for particle collection
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H

#include <memory>
#include <string_view>

#include "core/globals.h"
#include "objects/object.h"
#include "particles/particle.h"

// Log the particle's energy when it intersects the detector volume, deleting it afterwards
void logEnergyIfInside(std::unique_ptr<Particle>& particle,
                       const Object* detector,
                       const std::string_view &baseFolder = Globals::Constant::Program::outputFolder,
                       const std::string_view &baseFilename = Globals::Constant::Program::filenamePrefix);

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_COLLECTION_H