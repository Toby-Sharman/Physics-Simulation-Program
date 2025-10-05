//
// Created by Tobias Sharman on 15/09/2025.
//

#include "particle_collection.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>

void logEnergyIfInside(std::unique_ptr<Particle>& p, const Box& box,
                       const std::string_view &baseFolder,
                       const std::string_view &baseFilename)
{
    if (!p) return; // already deleted

    Vector<4> pos4 = p->getPosition();

    if (const auto pos3 = Vector<3>(pos4[1], pos4[2], pos4[3]); box.containsPoint(pos3)) {

        static std::map<std::string, std::unique_ptr<std::ofstream>> fileMap;
        static std::map<std::string, int> fileCounter;

        const std::string& type = p->getParticleName();

        if (!fileMap.contains(type)) {
            const std::filesystem::path folder = std::filesystem::path(baseFolder) / type;
            std::filesystem::create_directories(folder);

            int counter = 1;
            std::filesystem::path filename;
            do {
                filename = folder / std::format("{}{}{}", baseFilename, counter, ".csv");
                counter++;
            } while (std::filesystem::exists(filename));

            fileCounter[type] = counter;
            auto file = std::make_unique<std::ofstream>(filename, std::ios::app);
            if (!file->is_open()) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            fileMap[type] = std::move(file);
        }

        *(fileMap[type]) << p->getMomentum()[0].asDouble() << "\n";

        // Delete the particle after logging
        p.reset(); // deletes particle
    }
}
