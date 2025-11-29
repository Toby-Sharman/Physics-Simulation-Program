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
#include <optional>

#include "config/program_config.h"
#include "physics/processes/interaction_utilities.h"

namespace {
    constexpr double geometryTolerance = config::program::geometryTolerance;
    constexpr double fallbackScale = config::program::boundaryFallbackScale;
    constexpr double epsilonScale = config::program::boundaryEpsilonScale;

    struct IntersectionData {
        Vector<3> point;
        double fraction = 0.0;
        Vector<3> normal;
        bool hasNormal = false;
    };

    std::optional<IntersectionData> computeIntersection(
        const Object* surface,
        const Vector<3>& startWorld,
        const Vector<3>& displacementWorld
    ) {
        const auto localStart = surface->worldToLocalPoint(startWorld);
        const auto localDisplacement = surface->worldToLocalDirection(displacementWorld);
        const auto intersectionLocal = surface->localIntersection(localStart, localDisplacement);

        if (!intersectionLocal) {
            return std::nullopt;
        }

        const auto dispNorm2 = localDisplacement.lengthSquared().value;
        if (dispNorm2 <= geometryTolerance * geometryTolerance) {
            return std::nullopt;
        }

        const auto deltaLocal = *intersectionLocal - localStart;

        IntersectionData result{};
        result.point = surface->localToWorldPoint(*intersectionLocal);
        result.fraction = std::clamp(deltaLocal.dot(localDisplacement).value / dispNorm2, 0.0, 1.0);

        if (const auto localNormal = surface->localNormal(*intersectionLocal);
            localNormal.length().value > geometryTolerance) {
            result.normal = surface->localToWorldDirection(localNormal);
            result.hasNormal = true;
        }

        return result;
    }

    std::optional<Vector<3>> normaliseDirection(const Vector<3>& candidate) {
        const auto length = candidate.length();
        if (length.value <= 0.0) {
            return std::nullopt;
        }

        return candidate / std::max(length.value, geometryTolerance);
    }

    std::optional<Vector<3>> selectWorldNormal(
        const BoundaryEvent& event,
        const Vector<3>& eventDisplacement,
        const Particle& particle
    ) {
        if (event.hasNormal) {
            if (const auto normal = normaliseDirection(event.normal)) {
                return normal;
            }
        }

        const auto localIntersection = event.surface->worldToLocalPoint(event.intersection);
        const auto localNormal = event.surface->localNormal(localIntersection);
        if (const auto normal = normaliseDirection(event.surface->localToWorldDirection(localNormal))) {
            return normal;
        }

        if (const auto normal = normaliseDirection(-eventDisplacement)) {
            logInteractionWarning("BoundaryResponse",
                "Degenerate surface normal; used displacement direction");
            return normal;
        }

        if (const auto normal = normaliseDirection(particle.getMomentum())) {
            logInteractionWarning("BoundaryResponse",
                "Degenerate surface normal; used incoming momentum for boundary response");
            return normal;
        }

        logInteractionWarning("BoundaryResponse",
            "Failed to determine fallback normal; boundary response skipped");
        return std::nullopt;
    }
} // namespace

bool particleBoundaryConditions(
    const Particle& particle,
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

    if (displacement.lengthSquared().value <= geometryTolerance * geometryTolerance) {
        return false;
    }

    const auto endPosition = startPosition + displacement;
    const Object* endMedium = world->findObjectContaining(endPosition);

    if (startMedium == endMedium) {
        return false;
    }

    std::optional<IntersectionData> bestHit;
    const Object* hitSurface = nullptr;
    for (const Object* surface : {startMedium, endMedium}) {
        if (surface == nullptr) {
            continue;
        }

        if (const auto candidate = computeIntersection(surface, startPosition, displacement)) {
            if (!bestHit.has_value() || candidate->fraction < bestHit->fraction) {
                bestHit = candidate;
                hitSurface = surface;
            }
        }
    }

    if (!bestHit.has_value()) {
        // Fallback: trim step slightly to avoid getting stuck on boundary ambiguity
        // Scale the step so the move is roughly a few geometry tolerances long
        const double stepLength = displacement.length().value;
        constexpr double targetLength = geometryTolerance * fallbackScale;

        double fallbackTolerance = 0.0;
        if (stepLength > geometryTolerance) {
            fallbackTolerance = std::min(1.0, targetLength / stepLength);
        }

        dt = dt * fallbackTolerance;
        displacement = displacement * fallbackTolerance;
        event.surface = startMedium;
        event.intersection = startPosition + displacement;
        event.mediumBefore = startMedium;
        event.mediumAfter = particle.isReflective() ? startMedium : endMedium;
        event.normal = Vector<3>();
        event.hasNormal = false;
        event.nudgeIntoMediumAfter = !particle.isReflective();
        return true;
    }

    double clampedTolerance = std::clamp(bestHit->fraction, 0.0, 1.0);
    clampedTolerance = std::max(clampedTolerance, geometryTolerance);
    dt = dt * clampedTolerance;
    displacement = displacement * clampedTolerance;

    event.surface = hitSurface;
    event.intersection = bestHit->point;
    event.mediumBefore = startMedium;
    event.mediumAfter = particle.isReflective() ? startMedium : endMedium;
    event.normal = bestHit->normal;
    event.hasNormal = bestHit->hasNormal;
    event.nudgeIntoMediumAfter = !particle.isReflective();
    return true;
}

void processBoundaryResponse(Particle& particle,
    const BoundaryEvent& event,
    const Vector<3>& eventDisplacement,
    const Quantity& travelledDistance)
{
    if (event.surface == nullptr || travelledDistance.value <= 0.0) {
        return;
    }

    const auto worldNormalOpt = selectWorldNormal(event, eventDisplacement, particle);

    if (!worldNormalOpt.has_value()) {
        return;
    }

    const auto& worldNormal = *worldNormalOpt;

    auto epsilon = travelledDistance * epsilonScale;
    if (epsilon.value <= geometryTolerance) {
        epsilon = Quantity(geometryTolerance, travelledDistance.unit);
    }

    const auto displacementDot = eventDisplacement.dot(worldNormal).value;

    if (particle.isReflective()) {
        particle.reflectMomentumAcrossNormal(worldNormal);
        const double sign = displacementDot >= 0.0 ? -1.0 : 1.0;
        particle.setPosition(particle.getPosition() + worldNormal * (epsilon * sign));
        return;
    }

    if (event.nudgeIntoMediumAfter) {
        const double sign = displacementDot >= 0.0 ? 1.0 : -1.0;
        particle.setPosition(particle.getPosition() + worldNormal * (epsilon * sign));
    }
}
