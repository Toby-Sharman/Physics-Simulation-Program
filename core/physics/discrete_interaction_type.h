//
// Physics Simulation Program
// File: discrete_interaction_type.h
// Created by Tobias Sharman on 13/11/2025
//
// Description:
//   - Quick access handling of the interaction types
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_DISCRETE_INTERACTION_TYPE_H
#define PHYSICS_SIMULATION_PROGRAM_DISCRETE_INTERACTION_TYPE_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

enum class DiscreteInteractionType : std::uint8_t {
    None = 0,
    PhotonAbsorption,
    SpontaneousEmission,
    Count // Is auto set to the amount of interactions as long as is at the end
};

inline constexpr std::size_t k_discreteInteractionTypeCount = static_cast<std::size_t>(DiscreteInteractionType::Count);

template<typename Func>
constexpr void forEachDiscreteInteractionType(Func&& func, const bool includeNone = false) {
    const auto startIndex = includeNone ? 0u : 1u;
    for (std::size_t index = startIndex; index < k_discreteInteractionTypeCount; ++index) {
        func(static_cast<DiscreteInteractionType>(index));
    }
}

#endif //PHYSICS_SIMULATION_PROGRAM_DISCRETE_INTERACTION_TYPE_H