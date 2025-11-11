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

#include "physics/distributions.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <random>
#include <stdexcept>

#include "core/globals.h"
#include "core/quantities/units.h"
#include "core/random/random_manager.h"

Vector<3> sampleThermalVelocity(const Quantity& temperature, const Quantity& particleMass) {
    if (!std::isfinite(temperature.value) || temperature.value < 0.0) {
        throw std::invalid_argument(std::format(
            "sampleThermalVelocity: temperature must be finite and >= 0, received {}",
            temperature
        ));
    }

    if (!std::isfinite(particleMass.value) || particleMass.value <= 0.0) {
        throw std::invalid_argument(std::format(
            "sampleThermalVelocity: particle mass must be finite and > 0, received {}",
            particleMass
        ));
    }

    if (!Unit::hasTemperatureDimension(temperature.unit)) {
        throw std::invalid_argument(std::format(
            "sampleThermalVelocity: temperature must have Θ dimension (expected {}, received {})",
            Unit::temperatureDimension().toString(),
            temperature.unit.toString()
        ));
    }

    if (!Unit::hasMassDimension(particleMass.unit)) {
        throw std::invalid_argument(std::format(
            "sampleThermalVelocity: particle mass must have M dimension (expected {}, received {})",
            Unit::massDimension().toString(),
            particleMass.unit.toString()
        ));
    }

    auto& engine = random_manager::engine(random_manager::Stream::ThermalVelocities);

    const auto& boltzmannConstant = quantityTable().at("k_b");
    const auto thermalVariance = boltzmannConstant * temperature / particleMass;
    const auto thermalStdDev = thermalVariance.raisedTo(0.5);

    if (thermalStdDev.value <= 0.0) {
        return Vector<3>{
            Quantity(0.0, thermalStdDev.unit),
            Quantity(0.0, thermalStdDev.unit),
            Quantity(0.0, thermalStdDev.unit)
        };
    }

    std::normal_distribution gaussian(0.0, thermalStdDev.value);

    Vector<3> velocity{};
    for (std::size_t axis = 0; axis < 3; ++axis) {
        velocity[axis] = Quantity(gaussian(engine), thermalStdDev.unit);
    }
    return velocity;
}

Vector<3> sampleIsotropicDirection() {
    auto& engine = random_manager::engine(random_manager::Stream::SourceSampling);
    thread_local std::uniform_real_distribution cosineDistribution(-1.0, 1.0);
    thread_local std::uniform_real_distribution azimuthalDistribution(0.0, 2.0 * Globals::Constant::Maths::pi);

    const double cosTheta = cosineDistribution(engine);
    const double sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta)); // Derive from cos(Θ) to preserve unit length
    const double phi = azimuthalDistribution(engine);
    const double sinPhi = std::sin(phi);
    const double cosPhi = std::cos(phi);

    const double x = sinTheta * cosPhi;
    const double y = sinTheta * sinPhi;
    const double z = cosTheta;

    return {
        Quantity::dimensionless(x),
        Quantity::dimensionless(y),
        Quantity::dimensionless(z)
    };
}