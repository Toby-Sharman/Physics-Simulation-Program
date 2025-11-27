//
// Physics Simulation Program
// File: spontaneous_emission.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of spontaneous_emission.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/discrete/interactions/spontaneous_emission.h"

#include <algorithm>
#include <cmath>  // For std::abs ignore warning
#include <limits>
#include <memory>
#include <string>
#include <string_view>

#include "config/program_config.h"
#include "databases/particle-data/particle_database.h"
#include "particles/particle-types/photon.h"
#include "physics/distributions.h"
#include "physics/processes/interaction_utilities.h"

using discrete_interaction::InteractionChannel;

namespace {
    constexpr std::string_view k_spontaneousEmissionTag = "Spontaneous emission";
}

namespace discrete_interaction::spontaneous_emission {
    bool isApplicable(const Particle &, const Object *) {
        return false;
    }

    std::optional<InteractionChannel> buildChannel(const Particle &, const Object *) {
        return std::nullopt;
    }

    Quantity sampleLength(
        const Particle &particle,
        const Object *medium,
        const InteractionChannel &channel
    ) {
        (void) particle;
        (void) medium;
        (void) channel;
        return Quantity::dimensionless(std::numeric_limits<double>::infinity());
    }

    void apply(std::unique_ptr<Particle> &particle, const Object *medium) {
        if (!particle) {
            return;
        }

        (void) medium;

        struct ClearClockOnExit {
            std::unique_ptr<Particle> &handle;
            ~ClearClockOnExit() { if (handle) { handle->clearDecayClock(); } }
        } clearClock{particle};

        if (particle->getType() == "photon") {
            particle->clearDecayState();
            return;
        }

        const Vector<3> direction = sampleIsotropicDirection();
        const std::string photonType = "photon";
        if (!g_particleDatabase.contains(photonType)) {
            logInteractionWarning(k_spontaneousEmissionTag, "Photon definition missing; emission skipped.");
            particle->clearDecayState();
            return;
        }

        const auto c = speedOfLight();
        Quantity photonEnergy;
        if (particle->hasDecayEnergy()) {
            static bool loggedTaggedEmission = false;
            if (!loggedTaggedEmission) {
                logInteractionWarning(
                    k_spontaneousEmissionTag,
                    "Emitting 10% of stored decay energy (temporary diagnostic scaling)."
                );
                loggedTaggedEmission = true;
            }
            photonEnergy = particle->getDecayEnergy() * 0.1;
        }
        else {
            const auto restEnergy = particle->getRestMass() * c * c;
            const auto availableEnergy = particle->getEnergy() - restEnergy;
            const double energyTolerance = std::max(
                config::program::geometryTolerance * std::abs(restEnergy.value),
                config::program::geometryTolerance
            );

            if (availableEnergy.value <= energyTolerance) {
                logInteractionWarning(
                    k_spontaneousEmissionTag,
                    "Insufficient kinetic energy after tolerance adjustment; emission skipped."
                );
                particle->clearDecayState();
                return;
            }
            photonEnergy = availableEnergy;
        }

        if (photonEnergy.value <= 0.0) {
            logInteractionWarning(k_spontaneousEmissionTag, "Computed photon energy non-positive; emission skipped.");
            particle->clearDecayState();
            return;
        }

        const auto momentumMagnitude = photonEnergy / c;
        const auto momentumVector = direction * momentumMagnitude;

        auto emittedPhoton = std::make_unique<Photon>(
            photonType,
            particle->getTime(),
            particle->getPosition(),
            photonEnergy,
            momentumVector,
            Vector<4>()
        );
        emittedPhoton->setAlive(true);
        emittedPhoton->clearDecayState();

        particle = std::move(emittedPhoton);
    }
} // namespace discrete_interaction::spontaneous_emission
