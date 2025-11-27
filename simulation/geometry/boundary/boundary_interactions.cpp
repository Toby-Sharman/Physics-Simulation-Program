//
// Physics Simulation Program
// File: boundary_interactions.cpp
// Created by Tobias Sharman on 13/11/2025
//
// Description:
//   - Implementation of boundary_interactions.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "simulation/geometry/boundary/boundary_interactions.h"

#include <algorithm>
#include <cmath> // For std::abs and std::clamp ignore warning
#include <format>
#include <limits>
#include <optional>

#include "config/program_config.h"
#include "physics/processes/interaction_utilities.h"

namespace {
    std::optional<Vector<3>> safeIntersection(
        const Object* object,
        const Vector<3>& origin,
        const Vector<3>& displacement
    ) {
        try {
            return object->worldIntersection(origin, displacement);
        }
        catch (const std::exception& error) {
            logInteractionWarning(
                "BoundaryIntersection",
                std::format(
                    "Skipped intersection with '{}' after tolerance spill (likely grazing hit): {}",
                    object ? object->getName() : "unknown object",
                    error.what()
                )
            );
            return std::nullopt;
        }
    }
}

bool particleBoundaryConditions(
    const Object* world,
    const Object* startMedium,
    const Vector<3>& startPosition,
    Vector<3>& displacement,
    Quantity& dt,
    BoundaryEvent& event
) {
    if (world == nullptr || startMedium == nullptr) {
        return false;
    }

    const auto originalLength = displacement.length();
    if (originalLength.value <= config::program::geometryTolerance) {
        return false;
    }

    const auto endPosition = startPosition + displacement;
    if (startMedium->contains(endPosition)) {
        return false;
    }

    const auto nextObject = world->findObjectContaining(endPosition);
    if (startMedium == nextObject && nextObject != nullptr) {
        return false;
    }

    struct Candidate {
        Quantity fraction = Quantity::dimensionless(std::numeric_limits<double>::infinity());
        Vector<3> displacement{};
        const Object* surface = nullptr;
    }

    best{Quantity::dimensionless(std::numeric_limits<double>::infinity()), displacement, nullptr};

    const auto considerCandidate = [&](const Object* surface, const Vector<3>& truncatedDisplacement) {
        const auto truncatedLength = truncatedDisplacement.length();
        if (truncatedLength.value <= Globals::Constant::Program::geometryTolerance) {
            return;
        }

        auto fractionOfOriginalLength = truncatedLength / originalLength;
        fractionOfOriginalLength.value = std::clamp(fractionOfOriginalLength.value, 0.0, 1.0);

        if (fractionOfOriginalLength.value < best.fraction.value) {
            best.fraction = fractionOfOriginalLength;
            best.displacement = truncatedDisplacement;
            best.surface = surface;
        }
    };

    const Object* targetSurface = nextObject ? nextObject : startMedium;
    constexpr double tol = config::program::geometryTolerance;
    const auto localStart = targetSurface->worldToLocalPoint(startPosition);
    const auto localDisp = targetSurface->worldToLocalDirection(displacement);

    auto tryHit = [&](const Object* surface) -> std::optional<Object::RayHit> {
        if (!surface) { return std::nullopt; }
        const auto ls = surface->worldToLocalPoint(startPosition);
        const auto ld = surface->worldToLocalDirection(displacement);
        return surface->localRaycast(ls, ld, tol);
    };

    if (const auto hit = tryHit(targetSurface)) {
        const double clampedT = std::clamp(hit->t, 0.0, 1.0);
        dt = dt * clampedT;
        displacement = displacement * clampedT;
        event.surface = targetSurface;
        event.intersection = targetSurface->localToWorldPoint(hit->point);
        event.mediumAfter = nextObject;
        return true;
    }

    if (targetSurface != startMedium) {
        if (const auto hit = tryHit(startMedium)) {
            const double clampedT = std::clamp(hit->t, 0.0, 1.0);
            dt = dt * clampedT;
            displacement = displacement * clampedT;
            event.surface = startMedium;
            event.intersection = startMedium->localToWorldPoint(hit->point);
            event.mediumAfter = nextObject;
            return true;
        }
    }

    // If the medium changed but no hit was found, reflect at the start point with zero travel
    if (nextObject != nullptr) {
        static int logged = 0;
        if (logged < 5) {
            ++logged;
            logInteractionWarning(
                "BoundaryIntersection",
                std::format(
                    "Medium changed from '{}' to '{}' but no intersection found; reflecting at start.",
                    startMedium ? startMedium->getName() : "unknown",
                    nextObject ? nextObject->getName() : "unknown"
                )
            );
        }
        dt.value = 0.0;
        displacement = displacement * 0.0;
        event.surface = startMedium;
        event.intersection = startPosition;
        event.mediumAfter = nextObject;
        return true;
    }

    return false;
}

void processBoundaryResponse(Particle& particle,
    const BoundaryEvent& event,
    const Vector<3>& eventDisplacement,
    const Quantity& travelledDistance)
{
    constexpr double geometryTolerance = config::program::geometryTolerance;

    if (event.surface == nullptr || !particle.isReflective() || travelledDistance.value <= 0.0) {
        return;
    }

    const auto localIntersection = event.surface->worldToLocalPoint(event.intersection);
    const auto localNormal = event.surface->localNormal(localIntersection);
    Vector<3> worldNormal;
    const auto tryAssignUnitNormal = [&](const Vector<3>& candidate) -> bool {
        const auto length = candidate.length();
        if (length.value <= geometryTolerance) {
            return false;
        }
        worldNormal = candidate / length;
        return true;
    };

    bool haveNormal = false;
    if (localNormal.length().value > geometryTolerance) {
        const auto surfaceNormalWorld = event.surface->localToWorldDirection(localNormal);
        haveNormal = tryAssignUnitNormal(surfaceNormalWorld);
    }

    if (!haveNormal) {
        if (tryAssignUnitNormal(-eventDisplacement)) {
            haveNormal = true;
            logInteractionWarning("BoundaryResponse",
                "Degenerate surface normal; used displacement direction for reflection.");
        }
    }

    if (!haveNormal) {
        if (tryAssignUnitNormal(particle.getMomentum())) {
            haveNormal = true;
            logInteractionWarning("BoundaryResponse",
                "Degenerate surface normal; used incoming momentum for reflection.");
        }
    }

    if (!haveNormal) {
        logInteractionWarning("BoundaryResponse",
            "Failed to determine fallback normal; particle reflection skipped.");
        return;
    }

    particle.reflectMomentumAcrossNormal(worldNormal);

    auto epsilon = travelledDistance * 1e-6;
    if (epsilon.value <= geometryTolerance) {
        epsilon = Quantity(geometryTolerance, epsilon.unit);
    }

    const auto displacementDirection = eventDisplacement.dot(worldNormal);
    double sign = displacementDirection.value >= 0.0 ? 1.0 : -1.0;
    if (std::abs(displacementDirection.value) <= geometryTolerance) {
        const auto momentumDot = particle.getMomentum().dot(worldNormal);
        sign = momentumDot.value <= 0.0 ? 1.0 : -1.0;
    }

    const auto correction = worldNormal * (epsilon * sign);

    auto updatedPosition = particle.getPosition();
    updatedPosition[0] -= correction[0];
    updatedPosition[1] -= correction[1];
    updatedPosition[2] -= correction[2];
    particle.setPosition(updatedPosition);
}
