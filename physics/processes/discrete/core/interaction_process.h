//
// Physics Simulation Program
// File: interaction_process.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Defines the InteractionProcess interface and InteractionChannel primitive
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_INTERACTION_PROCESS_H
#define PHYSICS_SIMULATION_PROGRAM_INTERACTION_PROCESS_H

#include <memory>
#include <optional>
#include <string_view>

#include "core/quantities/quantity.h"
#include "objects/object.h"
#include "particles/particle.h"

namespace discrete_interaction {
    class InteractionProcess {
        public:
            struct Channel {
                const InteractionProcess* process = nullptr;
                Quantity macroscopicCrossSection;
            };

            virtual ~InteractionProcess() = default;

            // Identifier used for registry lookups
            [[nodiscard]] virtual std::string_view getName() const = 0;

            // Check if process is valid for a some particle and medium
            //
            // Later may be extended to support other conditionals are valid such as magnetic field
            [[nodiscard]] virtual bool isApplicable(const Particle& particle, const Object* medium) const = 0;

            // Builds a macroscopic interaction channel for the particle/material pair
            [[nodiscard]] virtual std::optional<Channel> buildChannel(
                const Particle& particle,
                const Object* medium
            ) const = 0;

            // Samples a mean-free-path for the supplied channel; returning infinity disables the process
            [[nodiscard]] virtual Quantity sampleLength(
                const Particle& particle,
                const Object* medium,
                const Channel& channel
                ) const = 0;

            // Applies the discrete process and updates the particle state accordingly
            //
            // Later will need better handling for proper particle creation and deletion in derived TODO
            virtual void apply(std::unique_ptr<Particle>& particle, const Object* medium) const = 0;
    };

    using InteractionChannel = InteractionProcess::Channel;
} // namespace discrete_interaction

#endif //PHYSICS_SIMULATION_PROGRAM_INTERACTION_PROCESS_H