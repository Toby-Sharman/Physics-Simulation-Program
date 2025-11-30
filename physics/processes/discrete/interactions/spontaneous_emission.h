//
// Physics Simulation Program
// File: spontaneous_emission.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Spontaneous emission discrete interaction
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_SPONTANEOUS_EMISSION_H
#define PHYSICS_SIMULATION_PROGRAM_SPONTANEOUS_EMISSION_H

#include <memory>
#include <optional>

#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_channels.h"

namespace discrete_interaction::spontaneous_emission {
    // Spontaneous emission does not currently have a propagating channel, since it is a decay mode based around time
    // not an interaction length, so these entry points report their "not applicable" equivalent while preserving the
    // interface
    bool isApplicable(const Particle &particle, const Object *medium);
    std::optional<InteractionChannel> buildChannel(const Particle &particle, const Object *medium);
    Quantity sampleLength(const Particle &particle, const Object *medium, const InteractionChannel &channel);

    // Promote an excited atom into a photon using any stored decay energy (or kinetic energy as a fallback), sampling
    // an isotropic emission direction TODO: Proper handling
    void apply(std::unique_ptr<Particle> &particle, const Object *medium, SpawnQueue &spawned);
} // namespace discrete_interaction::spontaneous_emission

#endif //PHYSICS_SIMULATION_PROGRAM_SPONTANEOUS_EMISSION_H
