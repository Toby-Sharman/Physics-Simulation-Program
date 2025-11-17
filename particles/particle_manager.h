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
//   - Container access:       getParticles(), empty()
//   - Global instance:        g_particleManager
class ParticleManager {
    public:
        ParticleManager() = default;

        void addParticle(std::unique_ptr<Particle> particle) {
            if (particle) {
                this->m_particles.push_back(std::move(particle));
            }
        }

        void addParticles(std::vector<std::unique_ptr<Particle>> newParticles) {
            for (auto& particle : newParticles) {
                addParticle(std::move(particle));
            }
        }

        std::vector<std::unique_ptr<Particle>>& getParticles() { return this->m_particles; }

        [[nodiscard]] bool empty() const { return this->m_particles.empty(); }

    private:
        std::vector<std::unique_ptr<Particle>> m_particles;
};

inline ParticleManager g_particleManager;

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H