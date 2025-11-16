//
// Physics Simulation Program
// File: interaction_process_registry.h
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Registry utilities that expose the active discrete interaction processes
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_INTERACTION_PROCESS_REGISTRY_H
#define PHYSICS_SIMULATION_PROGRAM_INTERACTION_PROCESS_REGISTRY_H

#include <string_view>
#include <vector>

#include "objects/object.h"
#include "particles/particle.h"
#include "physics/processes/discrete/core/interaction_process.h"

namespace discrete_interaction {
    // Returns a list of all the interaction processes registered at startup
    [[nodiscard]] const std::vector<const InteractionProcess*>& registeredInteractionProcesses();

    // Filters the registry for processes applicable to the supplied particle/material pair
    //
    // Later may be extended to support other conditionals are valid such as magnetic field
    [[nodiscard]] std::vector<const InteractionProcess*> determineActiveInteractionProcesses(
        const Particle& particle,
        const Object* medium);

    // Looks up a process by name
    //
    // returns nullptr when the process is unknown
    [[nodiscard]] const InteractionProcess* findInteractionProcess(std::string_view name);
} // namespace discrete_interaction

#endif //PHYSICS_SIMULATION_PROGRAM_INTERACTION_PROCESS_REGISTRY_H