//
// Physics Simulation Program
// File: photon_absorption.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Photon absorption discrete interaction
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PHOTON_ABSORPTION_H
#define PHYSICS_SIMULATION_PROGRAM_PHOTON_ABSORPTION_H

#include <optional>

#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_channels.h"

namespace discrete_interaction::photon_absorption {
    // Determine whether photon absorption is meaningful for the provided particle and medium
    bool isApplicable(const Particle &particle, const Object *medium);

    // Build the photon absorption interaction channel encapsulating the macroscopic cross-section
    std::optional<InteractionChannel> buildChannel(const Particle &particle, const Object *medium);

    // Sample a mean-free-path length for photon absorption from the provided interaction channel
    Quantity sampleLength(const Particle &particle, const Object *medium, const InteractionChannel &channel);

    // Apply photon absorption by validating the photon/medium pair, sampling the atom's thermal motion, and replacing
    // the photon with an excited state carrying the photon's stored energy
    void apply(std::unique_ptr<Particle> &particle, const Object *medium);
} // namespace discrete_interaction::photon_absorption

#endif //PHYSICS_SIMULATION_PROGRAM_PHOTON_ABSORPTION_H