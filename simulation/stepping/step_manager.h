//
// Physics Simulation Program
// File: step_manager.h
// Created by Tobias Sharman on 01/09/2025
//
// Description:
//   - Describe functions for simulation advancement
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_STEP_MANAGER_H
#define PHYSICS_SIMULATION_PROGRAM_STEP_MANAGER_H

#include "core/quantities/quantity.h"
#include "objects/object.h"

// All particle advancement
//
// Advance every particle by dt; particles logged inside the detector are removed afterwards
void stepAll(
    const Object* detector,
    const Quantity& dt = quantityTable().at("time step"));

#endif //PHYSICS_SIMULATION_PROGRAM_STEP_MANAGER_H