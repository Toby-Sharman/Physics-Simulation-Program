//
// Physics Simulation Program
// File: interaction_sampling.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Helpers for sampling discrete interaction lengths and selecting next events
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_INTERACTION_SAMPLING_H
#define PHYSICS_SIMULATION_PROGRAM_INTERACTION_SAMPLING_H

#include <cmath>
#include <limits>
#include <vector>

#include "core/quantities/quantity.h"
#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_channels.h"
#include "physics/processes/discrete/core/interaction_process.h"

namespace discrete_interaction {
    // Holds a channel paired with its sampled interaction length
    struct ChannelInteractionLength {
        InteractionChannel channel{};
        Quantity length = Quantity::dimensionless(0.0);
    };

    // Result of selecting the next discrete interaction (or none)
    struct InteractionSample {
        const InteractionProcess *process = nullptr;
        Quantity length = Quantity::dimensionless(std::numeric_limits<double>::infinity());

        [[nodiscard]] bool hasInteraction() const {
            return process != nullptr && std::isfinite(length.value) && length.value > 0.0;
        }
    };

    // Computes the sampled mean-free-path for every active channel
    [[nodiscard]] std::vector<ChannelInteractionLength> computeChannelInteractionLengths(
        const Particle &particle,
        const Object *medium
    );

    // Chooses the limiting interaction (if any) for the particle/material pair
    [[nodiscard]] InteractionSample sampleInteractionEvent(
        const Particle &particle,
        const Object *medium
    );
} // namespace discrete_interaction

#endif //PHYSICS_SIMULATION_PROGRAM_INTERACTION_SAMPLING_H