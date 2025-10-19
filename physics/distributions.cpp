//
// Physics Simulation program
// File: distributions.cpp
// Created by Tobias Sharman on 15/10/2025
//
// Description:
//   - Implementation of distributions.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include <random>

#include "physics/distributions.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"

[[nodiscard]] Vector<3> sampleThermalVelocity(const Quantity& temperature, const Quantity& particleMass) {
    std::random_device rd;
    std::ranlux48 gen(rd());

    const auto sigma = (quantityTable().at("k_b") * temperature / particleMass).raisedTo(0.5);

    std::normal_distribution<> normal(0.0, sigma.value);

    return {
        Quantity(normal(gen), sigma.unit),
        Quantity(normal(gen), sigma.unit),
        Quantity(normal(gen), sigma.unit)
    };
}