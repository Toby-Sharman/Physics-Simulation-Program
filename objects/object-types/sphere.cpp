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

#include "core/globals.h"
#include "core/maths/utilities/quantity.h"

void Sphere::setRadius(const Quantity& radius) {
    if (radius.unit != Unit(1,0,0,0,0,0,0)) {
        throw std::invalid_argument(std::format(
            "Sphere '{}' radius must have length dimensions but got {}",
            m_name,
            radius
        ));
    }
    this->m_radius = radius;
}

bool Sphere::isVolumeless() const noexcept {
    constexpr auto tolerance = Globals::Constant::Program::geometryTolerance;
    return m_radius.abs().value <= tolerance;
}

bool Sphere::contains(const Vector<3>& worldPoint) const {
    if (isVolumeless()) {
        return false;
    }

    const auto localRadius = worldToLocalPoint(worldPoint).length();
    const auto radiusSlack = m_radius.abs() * Globals::Constant::Program::geometryTolerance;

    return localRadius <= (m_radius + radiusSlack);
}

Vector<3> Sphere::localIntersection(const Vector<3> &startLocalPoint, const Vector<3> &localDisplacement) const {
    const auto stepLength = localDisplacement.length();
    if (stepLength.value == 0.0) {
        throw std::runtime_error(std::format(
            "Sphere '{}' cannot compute intersection because displacement length is zero (start = {}, displacement = {})",
            m_name,
            startLocalPoint,
            localDisplacement
        ));
    }

    constexpr auto tolerance = Globals::Constant::Program::geometryTolerance;

    const auto a = stepLength * stepLength;
    const auto b = 2.0 * startLocalPoint.dot(localDisplacement);
    const auto c = startLocalPoint.dot(startLocalPoint) - this->m_radius * this->m_radius;

    const auto bRatio = b / a; // Dimensionless
    const auto cRatio = c / a; // Dimensionless

    auto discriminant = bRatio.value * bRatio.value - 4.0 * cRatio.value;
    if (discriminant < -std::max(std::abs(discriminant), 1.0) * tolerance) {
        throw std::runtime_error(std::format(
            "Sphere '{}' intersection discriminant is negative (value = {}, start = {}, displacement = {})",
            m_name,
            discriminant,
            startLocalPoint,
            localDisplacement
        ));
    }
    discriminant = std::max(discriminant, 0.0);

    const double sqrtDisc = std::sqrt(discriminant);
    const double tEntry = (-bRatio.value - sqrtDisc) * 0.5;
    const double tExit = (-bRatio.value + sqrtDisc) * 0.5;

    const auto withinSegment = [](const double t) {
        return t >= -tolerance && t <= 1.0 + tolerance;
    };

    // Deal with numeric imprecision
    const auto clamp01 = [](const double t) {
        return std::max(0.0, std::min(1.0, t));
    };

    double t = 0.0;
    if (withinSegment(tEntry)) {
        t = clamp01(tEntry);
    } else if (withinSegment(tExit)) {
        t = clamp01(tExit);
    } else {
        throw std::runtime_error(std::format(
            "Sphere '{}' intersection parameter not in [0,1]: tEntry = {}, tExit = {} (start = {}, displacement = {})",
            m_name,
            tEntry,
            tExit,
            startLocalPoint,
            localDisplacement
        ));
    }

    return startLocalPoint + localDisplacement * t;
}

Vector<3> Sphere::localNormal(const Vector<3> &localPoint) const {
    if (isVolumeless()) {
        return {0.0, 0.0, 0.0};
    }
    return localPoint.unitVector(); // Outward unit vector
}

void Sphere::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, Unit(1,0,0,0,0,0,0));

    std::cout << pad << "Sphere: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Radius: " << m_radius << std::endl;
}