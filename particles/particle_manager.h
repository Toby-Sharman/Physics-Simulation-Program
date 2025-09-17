//
// Created by Tobias Sharman on 17/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H

#include "particle.h"
#include <memory>
#include <vector>

class ParticleManager {
public:
    ParticleManager() = default;

    // Add a single particle by forwarding construction arguments
    template<typename... Args>
    void addParticle(Args&&... args) {
        particles.push_back(std::make_unique<Particle>(std::forward<Args>(args)...));
    }

    // Add multiple particles from a vector
    void addParticles(const std::vector<Particle>& newParticles) {
        for (const auto& p : newParticles) {
            particles.push_back(std::make_unique<Particle>(p));
        }
    }

    std::vector<std::unique_ptr<Particle>>& getParticles() { return particles; }

    [[nodiscard]] bool empty() const { return particles.empty(); }

private:
    std::vector<std::unique_ptr<Particle>> particles;
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_MANAGER_H