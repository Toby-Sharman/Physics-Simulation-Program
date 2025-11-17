//
// Physics Simulation Program
// File: particle_manager.h
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Container that owns and manages active particles
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H

#include <memory>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "particles/particle.h"

// ParticleManager
//
// Notes on initialisation:
//   - Default-constructible holder with no ownership until particles are added
//
// Notes on algorithms:
//   - Stores particles as unique_ptr to enforce ownership and allow move-only semantics
//
// Supported overloads / operations and functions / methods:
//   - Add particles:          addParticle(), addParticles()
//   - Container access:       acquireReadHandle(), empty()
//   - Global instance:        g_particleManager
class ParticleManager {
    public:
        class ReadHandle {
            public:
                ReadHandle(std::vector<std::unique_ptr<Particle> > *particles, std::shared_mutex &mutex) :
                    m_particles(particles),
                    m_lock(mutex) {}

                ReadHandle(const ReadHandle &) = delete;

                ReadHandle &operator=(const ReadHandle &) = delete;

                ReadHandle(ReadHandle &&other) noexcept :
                    m_particles(other.m_particles),
                    m_lock(std::move(other.m_lock)) {
                    other.m_particles = nullptr;
                }

                ReadHandle &operator=(ReadHandle &&) noexcept = delete;

                ~ReadHandle() = default;

                [[nodiscard]] std::vector<std::unique_ptr<Particle> > &particles() const {
                    return *m_particles;
                }

            private:
                std::vector<std::unique_ptr<Particle> > *m_particles = nullptr;
                std::shared_lock<std::shared_mutex> m_lock;
        };

        ParticleManager() = default;

        void addParticle(std::unique_ptr<Particle> particle) {
            if (particle) {
                std::unique_lock lock(this->m_mutex);
                this->m_particles.push_back(std::move(particle));
            }
        }

        void addParticles(std::vector<std::unique_ptr<Particle> > newParticles) {
            std::unique_lock lock(this->m_mutex);
            for (auto &particle: newParticles) {
                if (particle) {
                    this->m_particles.push_back(std::move(particle));
                }
            }
        }

        [[nodiscard]] ReadHandle acquireReadHandle() {
            return ReadHandle(&this->m_particles, this->m_mutex);
        }

        template<typename Callable>
        void withExclusiveAccess(Callable &&callable) {
            std::unique_lock lock(this->m_mutex);
            callable(this->m_particles);
        }

        [[nodiscard]] bool empty() const {
            std::shared_lock lock(this->m_mutex);
            return this->m_particles.empty();
        }

    private:
        std::vector<std::unique_ptr<Particle> > m_particles;
        mutable std::shared_mutex m_mutex;
};

inline ParticleManager g_particleManager;

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H