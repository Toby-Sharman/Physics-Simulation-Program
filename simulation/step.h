//
// Created by Tobias Sharman on 01/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_STEP_H
#define PHYSICS_SIMULATION_PROGRAM_STEP_H

#include "globals.h"
#include "particle.h"

void step(const std::shared_ptr<Box>& collectionBox, Particle& p, double dt = Globals::Constant::TIME_STEP);

#endif //PHYSICS_SIMULATION_PROGRAM_STEP_H
