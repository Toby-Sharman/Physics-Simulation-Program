//
// Created by Tobias Sharman on 01/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_STEP_H
#define PHYSICS_SIMULATION_PROGRAM_STEP_H

#include "particle.h"
#include "core/maths/utilities/quantity.h"
#include "objects/object-types/box.h"

#include <memory>

void step(std::unique_ptr<Particle>& p, const std::shared_ptr<Box>& world, const std::shared_ptr<Box>& collectionBox, const Quantity& dt = quantityTable().at("time step"));

// Step all particles in a vector
void stepAll(std::vector<std::unique_ptr<Particle>>& particles, const std::shared_ptr<Box>& world, const std::shared_ptr<Box>& collectionBox, const Quantity& dt = quantityTable().at("time step"));

#endif //PHYSICS_SIMULATION_PROGRAM_STEP_H
