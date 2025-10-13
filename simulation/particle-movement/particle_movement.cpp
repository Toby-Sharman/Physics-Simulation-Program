//
// Created by Tobias Sharman on 17/09/2025.
//

#include "particle_movement.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"

void moveParticle(Particle& particle, const Quantity& dt) {
    auto& momentum4Vector = particle.getMomentum();
    const auto& energy = momentum4Vector[0];
    const auto& momentum = *reinterpret_cast<Vector<3> const*>(&momentum4Vector[1]);
    auto& initialPosition4Vector = particle.getPosition();
    const auto& initialTime = initialPosition4Vector[0];
    const auto& initialPosition = *reinterpret_cast<Vector<3> const*>(&initialPosition4Vector[1]);
    Vector<4> finalPosition;

    if (particle.isMassless()) {
        const auto displacement = momentum.unitVector() * dt * quantityTable().at("c");

        for (size_t i = 0; i < 3; ++i) {
            finalPosition[i + 1] = initialPosition[i] + displacement[i];
        }
        finalPosition[0] = initialTime + dt;
        particle.setPosition(finalPosition);
    } else { // massive particle
        const auto displacement = momentum / energy * dt * quantityTable().at("c");

        for (size_t i = 0; i < 3; ++i) {
            finalPosition[i + 1] = initialPosition[i] + displacement[i];
        }
        finalPosition[0] = initialTime + dt;
        particle.setPosition(finalPosition);
    }
}