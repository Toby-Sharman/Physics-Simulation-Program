//
// Created by Tobias Sharman on 17/09/2025.
//

#include "particle_movement.h"
#include "particle_interaction.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"

Vector<4> displacement(const Particle& particle, const Quantity dt) {
    const auto& momentum4Vector = particle.getMomentum();
    const auto& momentum = *reinterpret_cast<const Vector<3>*>(&momentum4Vector[1]);

    Vector<4> displacement;

    const auto c = quantityTable().at("c");
    const auto lightDistance = dt * c;

    if (particle.isMassless()) {
        const auto spatialDisplacement = momentum.unitVector() * lightDistance;

        displacement[0] = lightDistance;
        for (size_t i = 0; i < 3; ++i) {
            displacement[i + 1] = spatialDisplacement[i];
        }

        return displacement;
    }

    // Massive particle
    const auto& energy = momentum4Vector[0];
    const auto spatialDisplacement = momentum / energy * dt * c;

    displacement[0] = lightDistance;
    for (size_t i = 0; i < 3; ++i) {
        displacement[i + 1] = spatialDisplacement[i];
    }

    return displacement;
}


void moveParticle(Particle& particle, const Object*& world, const Quantity& dt) {
    const auto movementVector = displacement(particle, dt);
    //interaction(Particle& particle, movementVector);
    particle.setPosition(movementVector + particle.getPosition());
}