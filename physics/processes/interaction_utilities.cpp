//
// Physics Simulation Program
// File: interaction_utilities.cpp
// Created by Tobias Sharman on 12/11/2025
//
// Description:
//   - Implementation of interaction_utilities.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/interaction_utilities.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "config/program_config.h"
#include "core/quantities/units.h"

const Quantity& speedOfLight() {
    static const Quantity c = quantityTable().at("c");
    return c;
}

void logInteractionWarning(const std::string_view tag, const std::string_view message) {
    std::cerr << "[" << tag << "] " << message << '\n';
}

void clampVelocityToSubLuminal(Vector<3>& velocity,
    const std::string_view contextLabel,
    const Quantity& c) {
    const auto magnitude = velocity.length();
    if (magnitude.value <= 0.0 || c.value <= 0.0) {
        return;
    }

    const auto beta = magnitude / c;
    if (beta.value < 1.0) {
        return;
    }

    constexpr double maxBeta = 0.999999;
    const double scale = maxBeta / beta.value;
    velocity *= scale;

    if (const auto clampedMagnitude = velocity.length(); clampedMagnitude.value > c.value) {
        velocity *= c.value / clampedMagnitude.value;
    }

    logInteractionWarning(contextLabel, "Sampled velocity exceeded c; clamped to subluminal speed");
}

Quantity lorentzGammaFromSpeed(const Quantity& speed, const Quantity& c) {
    if (c.value <= 0.0) {
        throw std::invalid_argument("lorentzGammaFromSpeed: speed of light must be positive");
    }

    if (speed.value < 0.0) {
        throw std::invalid_argument("lorentzGammaFromSpeed: speed must be non-negative");
    }

    const double beta = (speed / c).value;
    const double clampedBeta = std::clamp(beta, 0.0, 1.0);
    const double oneMinusBetaSq = std::max(1e-12, 1.0 - clampedBeta * clampedBeta);
    const double gammaValue = 1.0 / std::sqrt(oneMinusBetaSq);
    constexpr double gammaLimit = config::program::lorentzGammaLimit;
    return Quantity::dimensionless(std::min(gammaValue, gammaLimit));
}
