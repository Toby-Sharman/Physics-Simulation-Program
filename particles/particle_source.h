//
// Created by Tobias Sharman on 17/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_SOURCE_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_SOURCE_H

#include "vector.h"

#include "particle.h"

#include <vector>
#include <random>
#include <utility>
#include <type_traits>

template <typename T>
constexpr auto range(T base, T spread) {
    return std::pair<T, T>{std::move(base), std::move(spread)};
}

class ParticleSource {
    public:
    template <typename PosT, typename MomT, typename PolT>
    std::vector<Particle> generateParticles(
        const std::string& particle_name,
        const int count,
        const PosT& position,
        const MomT& momentum,
        const PolT& polarisation
    ) {
        std::vector<Particle> particles;
        particles.reserve(count);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(-1.0, 1.0);

        for (int i = 0; i < count; ++i) {
            Vector<4> pos{};
            Vector<4> mom{};
            Vector<3> pol{};

            // --- Position ---
            if constexpr (std::is_same_v<PosT, Vector<4>>) {
                pos = position;
            } else if constexpr (std::is_same_v<PosT, std::pair<Vector<4>, Vector<4>>>) {
                pos = position.first;
                for (int j = 0; j < 4; ++j)
                    pos[j] += position.second[j] * dist(gen);
            }

            // --- Momentum ---
            if constexpr (std::is_same_v<MomT, Vector<4>>) {
                mom = momentum;
            } else if constexpr (std::is_same_v<MomT, std::pair<Vector<4>, Vector<4>>>) {
                mom = momentum.first;
                for (int j = 0; j < 4; ++j)
                    mom[j] += momentum.second[j] * dist(gen);
            }

            // --- Polarisation ---
            if constexpr (std::is_same_v<PolT, Vector<3>>) {
                pol = polarisation;
            } else if constexpr (std::is_same_v<PolT, std::pair<Vector<3>, Vector<3>>>) {
                pol = polarisation.first;
                for (int j = 0; j < 3; ++j)
                    pol[j] += polarisation.second[j] * dist(gen);
            }

            particles.emplace_back(particle_name, pos, mom, pol);
        }

        return particles;
    }
};


#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_SOURCE_H