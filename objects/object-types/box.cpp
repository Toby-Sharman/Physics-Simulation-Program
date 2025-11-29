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
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string>

#include "config/program_config.h"
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
    constexpr auto tolerance = config::program::geometryTolerance;

    return std::ranges::any_of(m_size, [](const Quantity& axis) {
        return axis.abs().value <= tolerance;
    });
}

bool Box::contains(const Vector<3>& worldPoint) const {
    if (isVolumeless()) {
        return false;
    }

    constexpr double tolerance = config::program::geometryTolerance;

    const auto local = worldToLocalPoint(worldPoint);
    const auto halfSize = this->m_size * 0.5;

    for (std::size_t i = 0; i < 3; ++i) {
        if (local[i].abs() > halfSize[i] + halfSize[i].abs() * tolerance) {
            return false;
        }
    }

    return true;
}

std::optional<Vector<3>> Box::localIntersection(const Vector<3>& startLocalPoint, const Vector<3>& localDisplacement) const {
    constexpr double tolerance = config::program::geometryTolerance;

    const auto halfSize = this->m_size * 0.5;

    double tEnter = 0.0;
    double tExit = 1.0;
    bool startInside = true;

    for (int axis = 0; axis < 3; ++axis) {
        const double dir = localDisplacement[axis].value;
        const double start = startLocalPoint[axis].value;
        const double extent = halfSize[axis].abs().value;

        if (extent <= tolerance) {
            return std::nullopt; // Degenerate box
        }

        if (std::abs(start) > extent + tolerance) {
            startInside = false;
        }

        if (std::abs(dir) <= tolerance) {
            // Parallel to this slab; must already be inside to hit
            if (std::abs(start) > extent + tolerance) {
                return std::nullopt;
            }
            continue; // Parallel but inside slab -> no constraint
        }

        double t0 = (-extent - start) / dir;
        double t1 = ( extent - start) / dir;
        if (t0 > t1) {
            std::swap(t0, t1);
        }

        tEnter = std::max(tEnter, t0);
        tExit = std::min(tExit, t1);
        if (tEnter - tExit > tolerance) {
            return std::nullopt; // Slabs do not overlap
        }
    }

    double tHit = startInside ? tExit : tEnter;
    if (!std::isfinite(tHit) || tHit < -tolerance || tHit > 1.0 + tolerance) {
        return std::nullopt;
    }

    tHit = std::clamp(tHit, 0.0, 1.0);

    return startLocalPoint + localDisplacement * tHit;
}

Vector<3> Box::localNormal(const Vector<3> &localPoint) const {
    if (isVolumeless()) {
        std::cout
        << std::format("Degenerate (point-like) objects like {} do not have a meaningful surface normal",
                this->m_name)
        << std::endl;
        return {0.0, 0.0, 0.0};
    }

    const Vector<3> halfSize = this->m_size * 0.5;
    double closestDistance = std::numeric_limits<double>::infinity();
    int dominantAxis = -1;
    int dominantSign = 0;

    for (int i = 0; i < 3; ++i) {
        const double extent = halfSize[i].abs().value;
        if (extent <= 0.0) { continue; }
        if (const double distanceToFace = extent - std::abs(localPoint[i].value); distanceToFace < closestDistance) {
            closestDistance = distanceToFace;
            dominantAxis = i;
            dominantSign = localPoint[i].value >= 0.0 ? 1 : -1;
        }
    }

    if (dominantAxis < 0 || !std::isfinite(closestDistance)) {
        return {0.0, 0.0, 0.0};
    }

    Vector<3> n = {0.0, 0.0, 0.0};
    n[dominantAxis] = Quantity::dimensionless(dominantSign);
    return n;
}

void Box::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, Unit::lengthDimension());

    std::cout << pad << "Box: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Size: " << m_size << std::endl;
}
