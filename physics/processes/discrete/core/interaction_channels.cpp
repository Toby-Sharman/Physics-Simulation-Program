//
// Physics Simulation Program
// File: interaction_channels.cpp
// Created by Tobias Sharman on 15/10/2025
//
// Description:
//   - Implementation of interaction_channels.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/discrete/core/interaction_channels.h"

#include <format>
#include <limits>
#include <stdexcept>

#include "core/random/random_manager.h"
#include "physics/processes/discrete/core/interaction_process_registry.h"

namespace discrete_interaction {
    std::vector<InteractionChannel> buildInteractionChannels(const Particle &particle, const Object *medium) {
        std::vector<InteractionChannel> channels;

        if (medium == nullptr) {
            return channels;
        }

        for (const auto *process : determineActiveInteractionProcesses(particle, medium)) {
            if (process == nullptr) {
                continue;
            }

            if (const auto channel = process->buildChannel(particle, medium); channel.has_value()) {
                channels.push_back(*channel);
            }
        }

        return channels;
    }

    Quantity totalMacroscopicCrossSection(const std::vector<InteractionChannel> &channels) {
        if (channels.empty()) {
            return Quantity::dimensionless(0.0);
        }

        Quantity total = channels.front().macroscopicCrossSection;
        for (std::size_t i = 1; i < channels.size(); ++i) {
            total += channels[i].macroscopicCrossSection;
        }

        if (total.value < 0.0) {
            throw std::runtime_error(
                std::format(
                    "Total macroscopic cross section is negative (value = {}, channels = {})",
                    total,
                    channels.size()
                )
            );
        }
        return total;
    }

    Quantity infiniteInteractionLength() {
        return {std::numeric_limits<double>::infinity(), Unit::lengthDimension()};
    }

    std::ranlux48 &rng() {
        return random_manager::engine(random_manager::Stream::DiscreteInteractions);
    }
} // namespace discrete_interaction