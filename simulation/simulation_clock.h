//
// Physics Simulation Program
// File: simulation_clock.h
// Created by Tobias Sharman on 29/11/2025
//
// Description:
//   - Global simulation clock helpers for tracking absolute simulation time
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_SIMULATION_CLOCK_H
#define PHYSICS_SIMULATION_PROGRAM_SIMULATION_CLOCK_H

#include "core/quantities/quantity.h"

namespace simulation_clock {
    // Current absolute simulation time (thread-safe copy)
    [[nodiscard]] Quantity currentTime();

    // Set the simulation clock to a specific absolute time
    void setTime(const Quantity& time = Quantity(0.0, Unit::timeDimension()));

    // Convenience alias for setTime to set the global absolute time to 0
    inline void reset(const Quantity& time = Quantity(0.0, Unit::timeDimension())) { setTime(time); }

    // Advance the simulation clock by dt and return the new absolute time
    [[nodiscard]] Quantity advance(const Quantity& dt);
} // namespace simulation_clock

#endif //PHYSICS_SIMULATION_PROGRAM_SIMULATION_CLOCK_H
