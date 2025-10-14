//
// Created by Tobias Sharman on 01/09/2025.
//

#include "step.h"

#include "particle_movement.h"

#include "particle_collection.h"

void step(std::unique_ptr<Particle>& particles, const std::shared_ptr<Box>& world, const std::shared_ptr<Box>& collectionBox, const Quantity& dt) {
    if (!particles) {
        return; // already deleted
    }
    moveParticle(*particles, world, dt); // Move particle
    logEnergyIfInside(particles, *collectionBox); // Log energy and delete particle if inside box
}

void stepAll(std::vector<std::unique_ptr<Particle>>& particles, const std::shared_ptr<Box>& world, const std::shared_ptr<Box>& collectionBox, const Quantity& dt) {
    for (auto& particle : particles) {
        step(particle, world, collectionBox,  dt);
    }

    std::erase(particles, nullptr); // Remove all deleted particles
}