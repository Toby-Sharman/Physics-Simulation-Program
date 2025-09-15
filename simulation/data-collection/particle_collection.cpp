//
// Created by Tobias Sharman on 15/09/2025.
//

#include "particle_collection.h"

#include <fstream>
#include <iostream>

void logEnergyIfInside(const Particle& p, const Box& box, const std::string& filename) {
    Vector<4> pos4 = p.getPosition();

    // Check if particle is inside the box
    if (const auto pos3 = Vector<3>({ pos4[1], pos4[2], pos4[3] }); box.containsPoint(pos3)) {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        // Write energy (E = p[0]) to file, one value per line
        file << p.getMomentum()[0] << "\n";
    }
}