//
// Created by Tobias Sharman on 15/09/2025.
//

#include "particle_collection.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>
#include <string>

void logEnergyIfInside(const Particle& p, const Box& box, const std::string& baseFolder, const std::string& baseFilename) {
    Vector<4> pos4 = p.getPosition();

    if (const auto pos3 = Vector<3>({ pos4[1], pos4[2], pos4[3] }); box.containsPoint(pos3)) {

        // Static map: one ofstream per particle type
        static std::map<std::string, std::unique_ptr<std::ofstream>> fileMap;
        // Static map: track next file number per particle type
        static std::map<std::string, int> fileCounter;

        const std::string& type = p.getParticleType();

        // If this particle type hasn't been logged yet, create a new numbered file
        if (!fileMap.contains(type)) {
            const std::filesystem::path folder = std::filesystem::path(baseFolder) / type;
            std::filesystem::create_directories(folder);

            // Find the next available number
            int counter = 1;
            std::filesystem::path filename;
            do {
                filename = folder / (baseFilename + std::to_string(counter) + ".csv");
                counter++;
            } while (std::filesystem::exists(filename));

            // Save counter for future runs (not strictly necessary, but keeps state)
            fileCounter[type] = counter;

            // Open the new file
            auto file = std::make_unique<std::ofstream>(filename, std::ios::app);
            if (!file->is_open()) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            fileMap[type] = std::move(file);
        }

        // Write energy to the already opened file
        *(fileMap[type]) << p.getMomentum()[0] << "\n";
    }
}

