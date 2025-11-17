//
// Physics Simulation Program
// File: photon_absorption.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of photon_absorption.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "physics/processes/discrete/interactions/photon_absorption.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <limits>
#include <memory>
#include <random>
#include <stdexcept>
#include <string_view>

#include "core/quantities/units.h"
#include "databases/material-data/material_database.h"
#include "databases/particle-data/particle_database.h"
#include "particles/particle-types/atom.h"
#include "physics/distributions.h"
#include "physics/processes/interaction_utilities.h"
#include "physics/processes/discrete/core/decay_utilities.h"

using discrete_interaction::InteractionChannel;

namespace {
    constexpr std::string_view k_photonAbsorptionTag = "Photon absorption";
} // namespace

namespace discrete_interaction::photon_absorption {
    bool isApplicable(const Particle &particle, const Object *medium) {
        if (medium == nullptr) {
            return false;
        }

        if (particle.getType() != "photon") {
            return false;
        }

        return !medium->getMaterial().empty();
    }

    std::optional<InteractionChannel> buildChannel(const Particle &particle, const Object *medium) {
        if (medium == nullptr || particle.getType() != "photon") {
            return std::nullopt;
        }

        const auto &material = medium->getMaterial();
        if (material.empty()) {
            return std::nullopt;
        }

        const auto numberDensity = g_materialDatabase.getNumberDensity(material);
        constexpr Quantity microscopicCrossSection(1e-18, Unit(2, 0, 0, 0, 0, 0, 0)); // TODO
        if constexpr (microscopicCrossSection.value <= 0.0) {
            return std::nullopt;
        }

        const auto macroscopic = numberDensity * microscopicCrossSection;
        if (macroscopic.value < 0.0) {
            throw std::runtime_error(
                std::format(
                    "Macroscopic cross section is negative for particle '{}' in material '{}' (value = {})",
                    particle.getType(),
                    material,
                    macroscopic
                )
            );
        }

        InteractionChannel channel{};
        channel.macroscopicCrossSection = macroscopic;
        return channel;
    }

    Quantity sampleLength(
        const Particle &particle,
        const Object *medium,
        const InteractionChannel &channel
    )
    {
        (void) particle;
        (void) medium;
        if (channel.macroscopicCrossSection.value <= 0.0) {
            return infiniteInteractionLength();
        }

        const auto meanFreePath = Quantity::dimensionless(1.0) / channel.macroscopicCrossSection;
        auto &rngEngine = rng();
        std::uniform_real_distribution uniform(0.0, 1.0);
        auto draw = uniform(rngEngine);
        draw = std::clamp(draw, std::numeric_limits<double>::min(), 1.0 - std::numeric_limits<double>::epsilon());
        const double opticalDepth = -std::log(draw);
        return meanFreePath * opticalDepth;
    }

    void apply(std::unique_ptr<Particle> &particle, const Object *medium) {
        if (!particle) { return; }

        if (particle->getType() != "photon" || !particle->getAlive()) {
            logInteractionWarning(k_photonAbsorptionTag, "Invalid particle state; interaction skipped.");
            return;
        }

        if (medium == nullptr) {
            logInteractionWarning(k_photonAbsorptionTag, "Medium not available; photon left unchanged.");
            return;
        }

        const auto &material = medium->getMaterial();
        if (material.empty()) {
            logInteractionWarning(k_photonAbsorptionTag, "Material not specified; photon left unchanged.");
            return;
        }

        if (!g_particleDatabase.contains(material)) {
            logInteractionWarning(
                k_photonAbsorptionTag,
                std::format(
                    "No particle definition for material '{}'; photon left unchanged.",
                    material
                )
            );
            return;
        }

        static bool warnedNonAtomicMaterial = false;
        if (g_particleDatabase.getParticleType(material) != ParticleType::Atom) {
            if (!warnedNonAtomicMaterial) {
                logInteractionWarning(
                    k_photonAbsorptionTag,
                    std::format(
                        "Material '{}' is not categorized as an atom; photon left unchanged.",
                        material
                    )
                );
                warnedNonAtomicMaterial = true;
            }
            return;
        }

        const auto restMass = g_particleDatabase.getRestMass(material);
        if (restMass.value <= 0.0) {
            logInteractionWarning(
                k_photonAbsorptionTag,
                std::format(
                    "Non-positive rest mass for particle '{}'; photon left unchanged.",
                    material
                )
            );
            return;
        }

        Vector<3> thermalVelocity{};
        try {
            thermalVelocity = sampleThermalVelocity(medium->getTemperature(), restMass);
        } catch (const std::exception &error) {
            logInteractionWarning(
                k_photonAbsorptionTag,
                std::format(
                    "Thermal velocity sampling failed: {}; photon left unchanged.",
                    error.what()
                )
            );
            return;
        }

        const auto c = speedOfLight();
        clampVelocityToSubLuminal(thermalVelocity, k_photonAbsorptionTag, c);

        const auto speed = thermalVelocity.length();
        const auto gamma = lorentzGammaFromSpeed(speed, c);

        auto matterMomentum = thermalVelocity * (restMass * gamma);
        const auto restEnergy = restMass * c * c;
        auto totalEnergy = restEnergy * gamma;

        const auto incidentMomentum = particle->getMomentum();
        const auto incidentEnergy = particle->getEnergy();
        matterMomentum += incidentMomentum;
        totalEnergy += incidentEnergy;

        if (!std::isfinite(totalEnergy.value)) {
            logInteractionWarning(k_photonAbsorptionTag, "Combined energy not finite; photon left unchanged.");
            return;
        }

        for (std::size_t axis = 0; axis < 3; ++axis) {
            if (!std::isfinite(matterMomentum[axis].value)) {
                logInteractionWarning(k_photonAbsorptionTag, "Combined momentum not finite; photon left unchanged.");
                return;
            }
        }

        auto absorbedAtom = std::make_unique<Atom>(
            material,
            particle->getTime(),
            particle->getPosition(),
            totalEnergy,
            matterMomentum,
            Vector<3>());
        absorbedAtom->setDecayEnergy(incidentEnergy);
        absorbedAtom->setAlive(true);

        if (absorbedAtom->getLifetime().value > 0.0) {
            absorbedAtom->setDecayClock(sampleDecayTime(*absorbedAtom));
        } else {
            absorbedAtom->clearDecayClock();
        }

        particle = std::move(absorbedAtom);
    }
} // namespace discrete_interaction::photon_absorption