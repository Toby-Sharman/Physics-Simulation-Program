//
// Physics Simulation Program
// File: decay_utilities.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of decay_utilities.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/discrete/core/decay_utilities.h"

#include <algorithm>
#include <limits>
#include <random>

#include "core/random/random_manager.h"

namespace discrete_interaction {
    Quantity sampleDecayTime(const Particle &particle) {
        const auto lifetime = particle.getLifetime();
        if (lifetime.value <= 0.0) {
            return {std::numeric_limits<double>::infinity(), lifetime.unit};
        }

        auto &rng = random_manager::engine(random_manager::Stream::DiscreteInteractions);
        std::uniform_real_distribution uniform(0.0, 1.0);
        double u = uniform(rng);
        u = std::clamp(u, std::numeric_limits<double>::min(), 1.0 - std::numeric_limits<double>::epsilon());

        const double factor = -std::log(u);
        return lifetime * factor;
    }
} // namespace discrete_interaction