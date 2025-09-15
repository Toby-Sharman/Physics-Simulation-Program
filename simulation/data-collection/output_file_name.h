//
// Created by Tobias Sharman on 15/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_OUTPUT_FILE_NAME_H
#define PHYSICS_SIMULATION_PROGRAM_OUTPUT_FILE_NAME_H

#include "globals.h"

#include <filesystem>
#include <string>
#include <sstream>

inline std::string generateSequentialFilename(const std::string& folder = "Output", const std::string& prefix = Globals::Constant::FILENAME_PREFIX) {
    std::filesystem::create_directories(folder); // Ensure Output exists

    int counter = 1;
    std::string filename;

    // Find the first filename that doesn't exist
    do {
        std::stringstream ss;
        ss << folder << "/" << prefix << counter << ".csv";
        filename = ss.str();
        counter++;
    } while (std::filesystem::exists(filename));

    return filename;
}


#endif //PHYSICS_SIMULATION_PROGRAM_OUTPUT_FILE_NAME_H