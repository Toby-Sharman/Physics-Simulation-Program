//
// Created by Tobias Sharman on 17/09/2025.
//

#include "particle_movement.h"

void moveParticle(Particle& p, double dt) {
    if (p.isMassless()) {
        const Vector<4> momentum = p.getMomentum();
        const Vector<4> initial_position = p.getPosition();
        Vector<4> final_position{};
        const double p_norm = std::sqrt(momentum[1].asDouble()*momentum[1].asDouble() +
                                        momentum[2].asDouble()*momentum[2].asDouble() +
                                        momentum[3].asDouble()*momentum[3].asDouble());
        if (p_norm != 0.0) {
            for (int i = 1; i < 4; ++i)
                final_position[i] = Quantity(initial_position[i].asDouble() + (momentum[i].asDouble() / p_norm) * dt * Globals::Constant::C);
        }
        final_position[0] = Quantity(initial_position[0].asDouble() + dt);
        p.setPosition(final_position);
    } else { // massive particle
        const Vector<4> initial_position = p.getPosition();
        Vector<4> final_position{};
        for (int i = 1; i < 4; ++i)
            final_position[i] = Quantity(initial_position[i].asDouble() +
                                         (p.getMomentum()[i].asDouble() / p.getMomentum()[0].asDouble()) * dt * Globals::Constant::C);
        final_position[0] = Quantity(initial_position[0].asDouble() + dt);
        p.setPosition(final_position);
    }
}