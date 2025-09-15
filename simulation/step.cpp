//
// Created by Tobias Sharman on 01/09/2025.
//

#include "vector.h"

#include "step.h"

#include <iostream>
#include <cmath>

void step(Particle& p, const double dt) {
    /*
     * dt default to TIME_STEP
     * Works on assumption c = 1
     */
    if (p.is_massless()) {
        // Photon: move at speed c=1 along momentum direction
        // Probably needless overcomplication but want to ensure that the program is robust
        std::cout << "MASSLESS\n";
        const Vector<4> momentum = p.get_momentum();
        const Vector<4> initial_position = p.get_position();
        Vector<4> final_position{};
        const double p_norm = std::sqrt(momentum[1] * momentum[1] +
                                  momentum[2] * momentum[2] +
                                  momentum[3] * momentum[3]);
        if (p_norm == 0.0) return;

        for (int i = 1; i < 4; ++i) {
            final_position[i] = initial_position[i] + (momentum[i] / p_norm) * dt * Globals::Constant::C;
        }
        final_position[0] = initial_position[0] + dt;
        p.set_position(final_position);
    } else {
        // Massive particle
        std::cout << "MASSIVE\n";
        const Vector<4> initial_position = p.get_position();
        Vector<4> final_position{};
        for (int i = 1; i < 4; ++i) {
            std::cout << p.get_momentum()[i] / p.get_momentum()[0] << ",";
            final_position[i] = initial_position[i] + (p.get_momentum()[i] / p.get_momentum()[0]) * dt * Globals::Constant::C;
        }
        std::cout << "\n";
        final_position[0] = initial_position[0] + dt;
        p.set_position(final_position);
    }
}