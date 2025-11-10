//
// Physics Simulation Program
// File: random_manager.h
// Created by Tobias Sharman on 02/11/2025
//
// Description:
//   - Provides thread safe random number generation with per-process streams
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_RANDOM_MANAGER_H
#define PHYSICS_SIMULATION_PROGRAM_RANDOM_MANAGER_H

#include <cstddef>
#include <cstdint>
#include <random>

namespace random_manager {

    // Enumerates the independent random-number streams used by each subsystem
    enum class Stream : std::uint32_t {
        Master = 0,
        DiscreteInteractions,
        ThermalVelocities,
        SourceSampling,
        UserDefined0
    };

    // Returns the current master seed
    //
    // Lazily initialising it if none has been set yet
    [[nodiscard]] std::uint64_t getMasterSeed();

    // Returns the effective seed for a stream
    //
    // Override if present, otherwise derived
    [[nodiscard]] std::uint64_t getStreamSeed(Stream stream);

    // Sets the master seed
    //
    // Passing zero falls back to a deterministic constant
    void setMasterSeed(std::uint64_t seed) noexcept;

    // Overrides the seed for one stream
    //
    // Zero values are hashed to avoid an all-zero seed
    void setStreamSeed(Stream stream, std::uint64_t seed) noexcept;

    // Provides the thread-local RNG engine for the given stream/index pair
    [[nodiscard]] std::ranlux48& engine(Stream stream, std::size_t streamIndex = 0);

    // Clears cached thread-local engines so new seeds take effect on the next use
    void resetCachedEngines() noexcept;
} // namespace random_manager

#endif //PHYSICS_SIMULATION_PROGRAM_RANDOM_MANAGER_H
