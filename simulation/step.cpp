//
// Created by Tobias Sharman on 01/09/2025.
//

#include "step.h"
#include "particle_movement.h"
#include "particle_collection.h"
#include "objects/object.h"

void step(std::unique_ptr<Particle>& particle, const Object* world, const Object* detector, const Quantity& dt) {
    if (!particle) {
        return; // already deleted
    }

    auto pos4 = particle->getPosition();

    if (const auto pos3 = *reinterpret_cast<Vector<3> const*>(&pos4[1]); !world->containsPoint(pos3)) {
        particle.reset(); // deletes particle
        return;
    }

    moveParticle(*particle, world, dt); // Move particle
    logEnergyIfInside(particle, detector); // Log energy and delete particle if inside box
}

void stepAll(std::vector<std::unique_ptr<Particle>>& particles, const Object* world, const Object* detector, const Quantity& dt) {
    for (auto& particle : particles) {
        step(particle, world, detector,  dt);
    }

    std::erase(particles, nullptr); // Remove all deleted particles
}