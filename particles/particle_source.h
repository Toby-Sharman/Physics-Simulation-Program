//
// Physics Simulation Program
// File: globals.h
// Created by Tobias Sharman on 17/09/2025.
//
// Description:
//   - Describes a particle generator that can allow for either consistent particle attribute generation or some randomness
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_SOURCE_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_SOURCE_H

#include "core/linear-algebra/vector.h"

#include "databases/particle-data/particle_database.h"
#include "particles/particle.h"
#include "particles/particle_manager.h"
#include "particles/particle-types/atom.h"
#include "particles/particle-types/photon.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

class ParticleSource { // TODO: Rework this logic is not very good to read. Also does not cover all I want from this class
    public:
    struct AtomGenerationConfig {
        std::vector<Atom::HyperfineLevel> hyperfineLevels;
        std::size_t activeLevelIndex = 0;
    };

    template <typename TimeT, typename PosT, typename EnergyT, typename MomT, typename PolT>
    void generateParticles(
        const std::string& particleName,
        const std::size_t count,
        const TimeT& timeSpec,
        const PosT& position,
        const EnergyT& energy,
        const MomT& momentum,
        const PolT& polarisation,
        const std::optional<AtomGenerationConfig>& atomConfig = std::nullopt
    ) {
        std::vector<std::unique_ptr<Particle>> particles;
        particles.reserve(count);

        std::random_device rd;
        std::ranlux48 gen(rd());
        std::uniform_real_distribution dist(-1.0, 1.0);

        const auto particleType = g_particleDatabase.getParticleType(particleName);
        using PolSpecT = std::decay_t<PolT>;
        constexpr bool polIsVector4 = std::is_same_v<PolSpecT, Vector<4>> || std::is_same_v<PolSpecT, std::pair<Vector<4>, Vector<4>>>;
        constexpr bool polIsVector3 = std::is_same_v<PolSpecT, Vector<3>> || std::is_same_v<PolSpecT, std::pair<Vector<3>, Vector<3>>>;
        const auto baseDrawCount =
            quantityDrawCount(timeSpec) +
            vectorDrawCount(position) +
            quantityDrawCount(energy) +
            vectorDrawCount(momentum);
        const auto polarisationDrawCount = vectorDrawCount(polarisation);

        for (std::size_t i = 0; i < count; ++i) {
            const std::size_t totalDraws =
                baseDrawCount +
                (particleType == ParticleType::Photon || particleType == ParticleType::Atom ? polarisationDrawCount : 0);
            std::vector<double> draws(totalDraws);
            for (auto& value : draws) {
                value = dist(gen);
            }
            std::size_t cursor = 0;

            const auto timeSample = sampleQuantityFromDraws(timeSpec, draws, cursor);
            const auto posSample = sampleVectorFromDraws(position, draws, cursor);
            const auto energySample = sampleQuantityFromDraws(energy, draws, cursor);
            const auto momSample = sampleVectorFromDraws(momentum, draws, cursor);

            if (particleType == ParticleType::Photon) {
                if constexpr (!polIsVector4) {
                    throw std::runtime_error("Photon sources require a Vector<4> polarisation specification.");
                } else {
                    const auto pol = sampleVectorFromDraws(polarisation, draws, cursor);
                    particles.push_back(std::make_unique<Photon>(
                        particleName,
                        timeSample,
                        posSample,
                        energySample,
                        momSample,
                        pol));
                }
            } else if (particleType == ParticleType::Atom) {
                if constexpr (!polIsVector3) {
                    throw std::runtime_error("Atom sources require a Vector<3> polarisation specification.");
                } else {
                    const auto pol = sampleVectorFromDraws(polarisation, draws, cursor);
                    auto atom = std::make_unique<Atom>(
                        particleName,
                        timeSample,
                        posSample,
                        energySample,
                        momSample,
                        pol);
                    if (atomConfig && !atomConfig->hyperfineLevels.empty()) {
                        atom->setHyperfineLevels(atomConfig->hyperfineLevels, atomConfig->activeLevelIndex);
                    }
                    particles.push_back(std::move(atom));
                }
            } else {
                particles.push_back(std::make_unique<Particle>(
                    particleName,
                    timeSample,
                    posSample,
                    energySample,
                    momSample));
            }
        }

        g_particleManager.addParticles(std::move(particles));
    }

    private:
        template <typename T>
        static constexpr bool k_dependentFalse = false;

        template <std::size_t N>
        static Vector<N> sampleVectorPair(const std::pair<Vector<N>, Vector<N>>& spec,
            const std::vector<double>& draws,
            std::size_t& cursor)
        {
            auto value = spec.first;
            for (std::size_t j = 0; j < N; ++j) {
                value[j] += spec.second[j] * draws[cursor++];
            }
            return value;
        }

        template <typename SpecT>
        static Quantity sampleQuantityFromDraws(const SpecT& spec,
            const std::vector<double>& draws,
            std::size_t& cursor)
        {
            if constexpr (std::is_same_v<std::decay_t<SpecT>, Quantity>) {
                (void)draws;
                (void)cursor;
                return spec;
            } else if constexpr (std::is_same_v<std::decay_t<SpecT>, std::pair<Quantity, Quantity>>) {
                return spec.first + spec.second * draws[cursor++];
            } else {
                static_assert(k_dependentFalse<SpecT>, "Unsupported quantity specification.");
                throw std::logic_error("Unsupported quantity specification.");
            }
        }

        template <typename SpecT>
        static std::size_t quantityDrawCount(const SpecT& spec) {
            if constexpr (std::is_same_v<std::decay_t<SpecT>, Quantity>) {
                (void)spec;
                return 0;
            } else if constexpr (std::is_same_v<std::decay_t<SpecT>, std::pair<Quantity, Quantity>>) {
                (void)spec;
                return 1;
            } else {
                static_assert(k_dependentFalse<SpecT>, "Unsupported quantity specification.");
                throw std::logic_error("Unsupported quantity specification.");
            }
        }

        template <typename SpecT>
        static std::size_t vectorDrawCount(const SpecT& spec) {
            using T = std::decay_t<SpecT>;
            if constexpr (std::is_same_v<T, Vector<3>> || std::is_same_v<T, Vector<4>>) {
                (void)spec;
                return 0;
            } else if constexpr (std::is_same_v<T, std::pair<Vector<3>, Vector<3>>>) {
                (void)spec;
                return 3;
            } else if constexpr (std::is_same_v<T, std::pair<Vector<4>, Vector<4>>>) {
                (void)spec;
                return 4;
            } else {
                static_assert(k_dependentFalse<SpecT>, "Unsupported vector specification.");
                throw std::logic_error("Unsupported vector specification.");
            }
        }

        template <typename SpecT>
        static auto sampleVectorFromDraws(const SpecT& spec,
            const std::vector<double>& draws,
            std::size_t& cursor)
        {
            using T = std::decay_t<SpecT>;
            if constexpr (std::is_same_v<T, Vector<3>> || std::is_same_v<T, Vector<4>>) {
                (void)draws;
                (void)cursor;
                return spec;
            } else if constexpr (std::is_same_v<T, std::pair<Vector<3>, Vector<3>>>) {
                return sampleVectorPair<3>(spec, draws, cursor);
            } else if constexpr (std::is_same_v<T, std::pair<Vector<4>, Vector<4>>>) {
                return sampleVectorPair<4>(spec, draws, cursor);
            } else {
                static_assert(k_dependentFalse<SpecT>, "Unsupported vector specification.");
                throw std::logic_error("Unsupported vector specification.");
            }
        }
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_SOURCE_H