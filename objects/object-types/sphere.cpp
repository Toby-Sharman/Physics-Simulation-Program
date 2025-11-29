//
// Physics Simulation Program
// File: sphere.cpp
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Implementation of sphere.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "objects/object-types/sphere.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

#include "config/program_config.h"

namespace {
    constexpr bool withinSegment(const double t) {
        constexpr double tolerance = config::program::geometryTolerance;
        return t >= -tolerance && t <= 1.0 + tolerance;
    }

    constexpr double clamp01(const double t) {
        if (t < 0.0) {
            return 0.0;
        }

        if (t > 1.0) {
            return 1.0;
        }

        return t;
    }
} // namespace

void Sphere::setRadius(const Quantity& radius) {
    if (!Unit::hasLengthDimension(radius.unit)) {
        throw std::invalid_argument(std::format(
            "Sphere '{}' radius must have length dimensions but got {}",
            m_name,
            radius
        ));
    }
    this->m_radius = radius;
}

bool Sphere::isVolumeless() const noexcept {
    constexpr auto tolerance = config::program::geometryTolerance;
    return m_radius.abs().value <= tolerance;
}

bool Sphere::contains(const Vector<3>& worldPoint) const {
    if (isVolumeless()) {
        return false;
    }

    constexpr double tolerance = config::program::geometryTolerance;

    const auto localRadius = worldToLocalPoint(worldPoint).length();
    const auto radiusSlack = m_radius.abs() * tolerance;

    return localRadius <= m_radius + radiusSlack;
}

std::optional<Vector<3>> Sphere::localIntersection(const Vector<3> &startLocalPoint, const Vector<3> &localDisplacement) const {
    constexpr auto tolerance = config::program::geometryTolerance;

    const double a = localDisplacement.dot(localDisplacement).value;
    const double b = 2.0 * startLocalPoint.dot(localDisplacement).value;
    const double c = startLocalPoint.dot(startLocalPoint).value - this->m_radius.value * this->m_radius.value;

    if (std::abs(a) <= tolerance) {
        return std::nullopt;
    }

    double discriminant = b * b - 4.0 * a * c;
    if (discriminant < -std::abs(discriminant) * tolerance) {
        return std::nullopt;
    }
    discriminant = std::max(discriminant, 0.0);

    const double sqrtDisc = std::sqrt(discriminant);
    const double tEntry = (-b - sqrtDisc) / (2.0 * a);
    const double tExit = (-b + sqrtDisc) / (2.0 * a);

    double t = std::numeric_limits<double>::infinity();
    if (withinSegment(tEntry)) {
        t = clamp01(tEntry);
    } else if (withinSegment(tExit)) {
        t = clamp01(tExit);
    }

    if (!std::isfinite(t)) {
        return std::nullopt;
    }

    return startLocalPoint + localDisplacement * t;
}

Vector<3> Sphere::localNormal(const Vector<3> &localPoint) const {
    if (isVolumeless()) {
        std::cout
        << std::format("Degenerate (point-like) objects like {} do not have a meaningful surface normal",
            this->m_name)
        << std::endl;
        return {0.0, 0.0, 0.0};
    }

    return localPoint.unitVector(); // Outward unit vector
}

void Sphere::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, Unit::lengthDimension());

    std::cout << pad << "Sphere: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Radius: " << m_radius << std::endl;
}
