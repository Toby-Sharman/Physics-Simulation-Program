//
// Physics Simulation Program
// File: simulation_clock.cpp
// Created by Tobias Sharman on 29/12/2025
//
// Description:
//   - Implementation of simulation_clock.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "simulation/simulation_clock.h"

#include <cmath>
#include <mutex>
#include <stdexcept>

#include "core/quantities/units.h"

namespace {
    std::mutex g_clockMutex;
    Quantity g_simulationTime{0.0, Unit::timeDimension()};

    // Extension on has time dimension to have proper handling for non-finite times
    void verifyTimeDimension(const Quantity& quantity, const char* context) {
        if (!Unit::hasTimeDimension(quantity.unit)) {
            throw std::invalid_argument(context);
        }

        if (!std::isfinite(quantity.value)) {
            throw std::invalid_argument("Simulation time must be finite");
        }
    }
} // namespace

Quantity simulation_clock::currentTime() {
    std::scoped_lock lock(g_clockMutex);

    return g_simulationTime;
}

void simulation_clock::setTime(const Quantity& time) {
    verifyTimeDimension(time, "Simulation time must have time dimensions");

    std::scoped_lock lock(g_clockMutex);

    g_simulationTime = time;
}

Quantity simulation_clock::advance(const Quantity& dt) {
    verifyTimeDimension(dt, "Simulation time step must have time dimensions");

    std::scoped_lock lock(g_clockMutex);

    g_simulationTime += dt;

    return g_simulationTime;
}
