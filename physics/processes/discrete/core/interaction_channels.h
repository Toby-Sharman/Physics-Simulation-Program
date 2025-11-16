//
// Physics Simulation Program
// File: interaction_channels.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Defines interaction channel sampling helpers used by discrete interactions
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_INTERACTION_CHANNELS_H
#define PHYSICS_SIMULATION_PROGRAM_INTERACTION_CHANNELS_H

#include <random>
#include <vector>

#include "core/quantities/quantity.h"
#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_process.h"

namespace discrete_interaction {
    // Builds the list of interaction channels applicable to the particle/material pair
    [[nodiscard]] std::vector<InteractionChannel> buildInteractionChannels(
        const Particle &particle,
        const Object *medium
    );

    // Computes the summed macroscopic cross-section across every supplied channel
    [[nodiscard]] Quantity totalMacroscopicCrossSection(const std::vector<InteractionChannel> &channels);

    // Utility helper returning an infinite interaction length with length dimension
    [[nodiscard]] Quantity infiniteInteractionLength();

    // Shared RNG used for discrete interaction sampling
    [[nodiscard]] std::ranlux48 &rng();
} // namespace discrete_interaction

#endif //PHYSICS_SIMULATION_PROGRAM_INTERACTION_CHANNELS_H