//
// Physics Simulation Program
// File: interaction_process_registry.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of interaction_process_registry.h and describe the different interaction processes as derived
//     classes of InteractionProcess
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/discrete/core/interaction_process_registry.h"

#include <string_view>
#include <vector>

#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_process.h"
#include "physics/processes/discrete/interactions/photon_absorption.h"
#include "physics/processes/discrete/interactions/spontaneous_emission.h"

// Unnamed namespace to limit to only being usable in this unit (i.e. cannot be accessed in another file)
//
// Describe different discrete interaction processes
namespace {
    using discrete_interaction::InteractionProcess;
    using discrete_interaction::InteractionChannel;

    class PhotonAbsorptionProcess final : public InteractionProcess {
        public:
            [[nodiscard]] std::string_view getName() const override { return "Photon absorption"; }

            [[nodiscard]] bool isApplicable(const Particle& particle, const Object* medium) const override {
                return discrete_interaction::photon_absorption::isApplicable(particle, medium);
            }

            [[nodiscard]] std::optional<InteractionChannel> buildChannel(
                const Particle& particle,
                const Object* medium) const override
            {
                auto channel = discrete_interaction::photon_absorption::buildChannel(particle, medium);
                if (channel.has_value()) {
                    channel->process = this;
                }
                return channel;
            }

            [[nodiscard]] Quantity sampleLength(const Particle& particle,
                const Object* medium,
                const InteractionChannel& channel) const override
            {
                return discrete_interaction::photon_absorption::sampleLength(particle, medium, channel);
            }

            void apply(std::unique_ptr<Particle>& particle, const Object* medium) const override {
                discrete_interaction::photon_absorption::apply(particle, medium);
            }
    };

    class SpontaneousEmissionProcess final : public InteractionProcess {
        public:
            [[nodiscard]] std::string_view getName() const override { return "Spontaneous emission"; }

            [[nodiscard]] bool isApplicable(const Particle& particle, const Object* medium) const override {
                return discrete_interaction::spontaneous_emission::isApplicable(particle, medium);
            }

            [[nodiscard]] std::optional<InteractionChannel> buildChannel(
                const Particle& particle,
                const Object* medium) const override
            {
                auto channel = discrete_interaction::spontaneous_emission::buildChannel(particle, medium);
                if (channel.has_value()) {
                    channel->process = this;
                }
                return channel;
            }

            [[nodiscard]] Quantity sampleLength(const Particle& particle,
                const Object* medium,
                const InteractionChannel& channel) const override
            {
                return discrete_interaction::spontaneous_emission::sampleLength(particle, medium, channel);
            }

            void apply(std::unique_ptr<Particle>& particle, const Object* medium) const override {
                discrete_interaction::spontaneous_emission::apply(particle, medium);
            }
    };
} // namespace

namespace discrete_interaction {
    const std::vector<const InteractionProcess*>& registeredInteractionProcesses() {
        static PhotonAbsorptionProcess photonProcess;
        static SpontaneousEmissionProcess spontaneousProcess;
        static const std::vector<const InteractionProcess*> processes = {
            &photonProcess,
            &spontaneousProcess
        };
        return processes;
    }

    std::vector<const InteractionProcess*> determineActiveInteractionProcesses(
        const Particle& particle,
        const Object* medium)
    {
        std::vector<const InteractionProcess*> active;
        if (medium == nullptr) {
            return active;
        }

        for (const auto* process : registeredInteractionProcesses()) {
            if (process != nullptr && process->isApplicable(particle, medium)) {
                active.push_back(process);
            }
        }
        return active;
    }

    const InteractionProcess* findInteractionProcess(const std::string_view name) {
        for (const auto* process : registeredInteractionProcesses()) {
            if (process != nullptr && process->getName() == name) {
                return process;
            }
        }
        return nullptr;
    }
} // namespace discrete_interaction