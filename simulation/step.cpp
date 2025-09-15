//
// Created by Tobias Sharman on 01/09/2025.
//

#include "vector.h"

#include "particle_collection.h"

#include "step.h"

#include <cmath>

void step(const std::shared_ptr<Box>& collectionBox, const std::string& filename, Particle& p, const double dt) {
    /*
     * dt default to TIME_STEP
     * Works on assumption c = 1
     */
    if (p.isMassless()) {
        // Photon: move at speed c=1 along momentum direction
        // Probably needless overcomplication but want to ensure that the program is robust
        const Vector<4> momentum = p.getMomentum();
        const Vector<4> initial_position = p.getPosition();
        Vector<4> final_position{};
        const double p_norm = std::sqrt(momentum[1] * momentum[1] +
                                  momentum[2] * momentum[2] +
                                  momentum[3] * momentum[3]);
        if (p_norm == 0.0) return;

        for (int i = 1; i < 4; ++i) {
            final_position[i] = initial_position[i] + (momentum[i] / p_norm) * dt * Globals::Constant::C;
        }
        final_position[0] = initial_position[0] + dt;
        p.setPosition(final_position);
    } else {
        // Massive particle
        const Vector<4> initial_position = p.getPosition();
        Vector<4> final_position{};
        for (int i = 1; i < 4; ++i) {
            final_position[i] = initial_position[i] + (p.getMomentum()[i] / p.getMomentum()[0]) * dt * Globals::Constant::C;
        }
        final_position[0] = initial_position[0] + dt;
        p.setPosition(final_position);
    }
    logEnergyIfInside(p, *collectionBox, filename);
}