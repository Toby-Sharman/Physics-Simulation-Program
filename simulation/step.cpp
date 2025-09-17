//
// Created by Tobias Sharman on 01/09/2025.
//

#include "step.h"

#include "particle_movement.h"

#include "particle_collection.h"

void step(const std::shared_ptr<Box>& collectionBox, std::unique_ptr<Particle>& p, double dt) {
    if (!p) return; // already deleted
    moveParticle(*p, dt); // Move particle
    logEnergyIfInside(p, *collectionBox); // Log energy and delete particle if inside box
}

void stepAll(const std::shared_ptr<Box>& collectionBox, std::vector<std::unique_ptr<Particle>>& particles, double dt) {
    for (auto& p : particles) {
        step(collectionBox, p, dt);
    }

    std::erase(particles, nullptr); // Remove all deleted particles
}