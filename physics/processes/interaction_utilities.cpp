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

#include <iostream>

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