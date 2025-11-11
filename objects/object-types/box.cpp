//
// Physics Simulation Program
// File: box.cpp
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Implementation of box.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "objects/object-types/box.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <string>

#include "core/globals.h"
#include "core/quantities/quantity.h"

void Box::setSize(const Vector<3> &size) {
    std::size_t axis = 0;
    for (const auto& dimension : size) {
        if (!Unit::hasLengthDimension(dimension.unit)) {
            throw std::invalid_argument(std::format(
                "Box '{}' size[{}] must have length dimensions but got {}",
                m_name,
                axis,
                dimension
            ));
        }
        ++axis;
    }
    this->m_size = size;
}

bool Box::isVolumeless() const noexcept {
    constexpr auto tolerance = Globals::Constant::Program::geometryTolerance;
    return std::ranges::any_of(m_size, [](const Quantity& axis) {
        return axis.abs().value <= tolerance;
    });
}

bool Box::contains(const Vector<3>& worldPoint) const {
    if (isVolumeless()) {
        return false;
    }

    const auto local = worldToLocalPoint(worldPoint);
    const auto halfSize = this->m_size * 0.5;
    constexpr auto tolerance = Globals::Constant::Program::geometryTolerance;

    for (std::size_t i = 0; i < 3; ++i) {
        if (local[i].abs() > halfSize[i] + halfSize[i].abs() * tolerance) {
            return false;
        }
    }

    return true;
}

Vector<3> Box::localIntersection(const Vector<3>& startLocalPoint, const Vector<3>& localDisplacement) const {
    const auto displacementLength = localDisplacement.length();
    if (displacementLength.value == 0.0) {
        throw std::runtime_error(std::format(
            "Box '{}' cannot compute intersection because displacement length is zero (start = {}, displacement = {})",
            m_name,
            startLocalPoint,
            localDisplacement
        ));
    }
    const Vector<3> halfSize = this->m_size * 0.5;
    constexpr auto relativeTolerance = Globals::Constant::Program::geometryTolerance;
    const auto absoluteTolerance = displacementLength * relativeTolerance;

    auto segmentEntry = 0.0; // Earliest parameter in [0, 1] where the ray enters every slab
    auto segmentExit = 1.0;  // Latest parameter before the ray exits the box
    bool startInside = true;

    for (std::size_t i = 0; i < 3; ++i) {
        const auto axisHalfExtent = halfSize[i];
        const Quantity axisDirection = localDisplacement[i]; // Displacement component along this axis
        const Quantity axisStart = startLocalPoint[i];       // Ray start along this axis
        const auto axisDirectionMagnitude = axisDirection.abs();
        if (const auto axisSlack = axisHalfExtent.abs() * relativeTolerance;
            axisStart < -axisHalfExtent - axisSlack || axisStart > axisHalfExtent + axisSlack) {
            startInside = false;
        }

        const bool nearlyParallel =
            axisDirectionMagnitude <= absoluteTolerance ||
            (axisDirectionMagnitude / displacementLength).value <= relativeTolerance;

        if (nearlyParallel) {
            if (axisStart < -axisHalfExtent - absoluteTolerance || axisStart > axisHalfExtent + absoluteTolerance) {
                throw std::runtime_error(std::format(
                    "Box '{}' segment lies outside slab on axis {} (start = {}, displacement = {}, slab half extent = {})",
                    m_name,
                    i,
                    startLocalPoint,
                    localDisplacement,
                    axisHalfExtent
                ));
            }
            continue;
        }

        // Compute t values for the two slab planes on this axis
        auto tNear = (-axisHalfExtent - axisStart) / axisDirection;
        auto tFar = ( axisHalfExtent - axisStart) / axisDirection;

        // Reorder to find which boundary the ray hits first
        const auto axisEntry = std::min(tNear.value, tFar.value);
        const auto axisExit = std::max(tNear.value, tFar.value);

        segmentEntry = std::max(segmentEntry, axisEntry);
        segmentExit = std::min(segmentExit, axisExit);

        if (segmentEntry > segmentExit) {
            throw std::runtime_error(std::format(
                "Box '{}' segment does not enter the volume (axis {} entry = {}, exit = {}, start = {}, displacement = {})",
                m_name,
                i,
                axisEntry,
                axisExit,
                startLocalPoint,
                localDisplacement
            ));
        }
    }

    const double parameter = startInside ? segmentExit : segmentEntry;
    if (parameter < -relativeTolerance || parameter > 1.0 + relativeTolerance) {
        throw std::runtime_error(std::format(
            "Box '{}' intersection parameter {} outside [0,1] (start = {}, displacement = {}, startInside = {})",
            m_name,
            parameter,
            startLocalPoint,
            localDisplacement,
            startInside
        ));
    }

    return startLocalPoint + localDisplacement * parameter;
}

Vector<3> Box::localNormal(const Vector<3> &localPoint) const {
    const Vector<3> halfSize = this->m_size * 0.5;
    const auto epsilon = halfSize.length() * Globals::Constant::Program::geometryTolerance;

    for (std::size_t i = 0; i < 3; ++i) {
        // Compare against positive face
        if ((localPoint[i] - halfSize[i]).abs() < epsilon) {
            Vector<3> n = {0.0, 0.0, 0.0};
            n[i] = Quantity::dimensionless(1.0); // unitless direction
            return n;
        }

        // Compare against negative face
        if ((localPoint[i] + halfSize[i]).abs() < epsilon) {
            Vector<3> n = {0.0, 0.0, 0.0};
            n[i] = Quantity::dimensionless(-1.0);
            return n;
        }
    }

    return {0.0, 0.0, 0.0}; // Should not happen unless point is off-box
}

void Box::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, Unit::lengthDimension());

    std::cout << pad << "Box: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Size: " << m_size << std::endl;
}