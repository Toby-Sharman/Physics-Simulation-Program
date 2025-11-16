//
// Physics Simulation Program
// File: interaction_utilities.h
// Created by Tobias Sharman on 12/11/2025
//
// Description:
//   - Utilities for interactions
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_INTERACTION_UTILITIES_H
#define PHYSICS_SIMULATION_PROGRAM_INTERACTION_UTILITIES_H

#include <string_view>

#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"

// TODO: Complete documentation of these functions

// Speed of light
//
// Get the speed of light. Currently, a cached version of fetching from the quantity table - but later this would shift
// to being based upon the material within for the slowed speed of light TODO
const Quantity& speedOfLight();

// Interaction warning helper
//
// Emit a standardized interaction warning. Makes writing all the interaction warnings quicker and easier
void logInteractionWarning(std::string_view tag, std::string_view message);

// Realistic velocity catcher
//
// Clamp a velocity vector to be strictly below c; logs warning with context label when clamping occurs
void clampVelocityToSubLuminal(Vector<3>& velocity,
    std::string_view contextLabel,
    const Quantity& c = speedOfLight());

// Lorentz factor helper
//
// Compute γ = 1 / sqrt(1 - (v/c)^2) for a supplied speed magnitude, guarding against superluminal inputs
Quantity lorentzGammaFromSpeed(const Quantity& speed, const Quantity& c = speedOfLight());

#endif //PHYSICS_SIMULATION_PROGRAM_INTERACTION_UTILITIES_H