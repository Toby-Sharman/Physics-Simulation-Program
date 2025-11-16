//
// Physics Simulation Program
// File: interaction_sampling.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of interaction_sampling.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/discrete/core/interaction_sampling.h"

#include <cmath>

#include "core/quantities/units.h"
#include "physics/processes/discrete/core/interaction_process_registry.h"

namespace discrete_interaction {
    std::vector<ChannelInteractionLength> computeChannelInteractionLengths(
        const Particle &particle,
        const Object *medium
    ) {
        std::vector<ChannelInteractionLength> lengths;
        if (medium == nullptr) {
            return lengths;
        }

        const auto channels = buildInteractionChannels(particle, medium);
        if (channels.empty()) {
            return lengths;
        }

        lengths.reserve(channels.size());
        for (const auto &channel: channels) {
            ChannelInteractionLength entry{};
            entry.channel = channel;
            if (entry.channel.process == nullptr) {
                continue;
            }
            entry.length = entry.channel.process->sampleLength(particle, medium, channel);
            lengths.push_back(entry);
        }
        return lengths;
    }

    InteractionSample sampleInteractionEvent(
        const Particle &particle,
        const Object *medium
    ) {
        InteractionSample result{};
        result.length = infiniteInteractionLength();

        const auto channelLengths = computeChannelInteractionLengths(particle, medium);
        if (channelLengths.empty()) {
            return result;
        }

        bool assigned = false;
        for (const auto &[channel, length]: channelLengths) {
            if (!std::isfinite(length.value) || length.value <= 0.0) {
                continue;
            }

            if (!assigned || length.value < result.length.value) {
                result.process = channel.process;
                result.length = length;
                assigned = true;
            }
        }

        if (!assigned) {
            result.process = nullptr;
            result.length = infiniteInteractionLength();
        }

        return result;
    }
} // namespace discrete_interaction