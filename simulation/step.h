//
// Created by Tobias Sharman on 01/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_STEP_H
#define PHYSICS_SIMULATION_PROGRAM_STEP_H

#include "globals.h"
#include "particle.h"
#include "box.h"

#include <memory>

void step(const std::shared_ptr<Box>& collectionBox, std::unique_ptr<Particle>& p, double dt = Globals::Constant::Program::timeStep);

// Step all particles in a vector
void stepAll(const std::shared_ptr<Box>& collectionBox, std::vector<std::unique_ptr<Particle>>& particles, double dt = Globals::Constant::Program::timeStep);

#endif //PHYSICS_SIMULATION_PROGRAM_STEP_H
