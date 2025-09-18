//
// Created by Tobias Sharman on 01/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_GLOBALS_H
#define PHYSICS_SIMULATION_PROGRAM_GLOBALS_H

#include <string>

class Globals {
    public:
    static Globals &Instance() {
        static Globals instance;
        return instance;
    };

    struct Constant {
        // Compile-time constants
        static constexpr std::string FILENAME_PREFIX = "Energies";            // Data output file name prefix (in form prefix[run#]
        static constexpr std::string OUTPUT_FOLDER = "Output";                // Data output file name prefix (in form prefix[run#]

        static constexpr double TIME_STEP = 1e-8;                             // Seconds
        static constexpr double MASSLESS_TOLERANCE = 0;                       // Massless tolerance

        static constexpr double PI = 3.141592653589793238462643383279502884L; // Pi
        static constexpr double DEG2RAD = PI / 180.0;                         // Degree to radian ratio
        static constexpr double RAD2DEG = 180.0 / PI;                         // Radian to degree ratio

        static constexpr double C = 299792458;                                // Speed of light
        static constexpr double MU0 = 1.25663706127e-6;                       // Vacuum permeability
        static constexpr double e = 1.602176634e-19;                          // Elementary charge
        static constexpr double u = 1.66053906892e-27;                        // Atomic mass unit TODO: Support for Da (Dalton) name scheme
        static constexpr double h = 6.62607015e-34;                           // Planck constant
        static constexpr double hbar = h/PI;                                  // Reduced Planck constant

    };

    // ========= Runtime constants =========
    // Stored in memory, set at runtime
    // Use Globals::instance().CONST_NAME
    // const std::string NAME = "name";

    // ========= Modifiable variables =========
    // Use Globals::instance().var_name
    // int counter = 0;

    // No copies allowed
    Globals(const Globals&) = delete;
    Globals& operator=(const Globals&) = delete;

private:
    ~Globals() = default;
    Globals() = default;  // Private constructor
};

#endif //PHYSICS_SIMULATION_PROGRAM_GLOBALS_H