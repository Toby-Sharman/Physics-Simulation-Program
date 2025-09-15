//
// Created by Tobias Sharman on 10/09/2025.
//

// Exponent handling is limited to integers otherwise handling is spotty

#ifndef PHYSICS_SIMULATION_PROGRAM_UNITS_H
#define PHYSICS_SIMULATION_PROGRAM_UNITS_H

#include "globals.h"

#include "unit_utilities.h"

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <utility>       // for std::pair
#include <ranges>
#include <format>
#include <stdexcept>     // std::invalid_argument

inline const std::unordered_map<std::string,double>& PREFIXES() {
    static std::unordered_map<std::string,double> table = {
        {"Q",1e30}, {"R",1e27}, {"Y",1e24}, {"Z",1e21}, {"E",1e18},
        {"P",1e15}, {"T",1e12}, {"G",1e9},  {"M",1e6},  {"k",1e3},
        {"h",1e2},  {"da",1e1}, {"d",1e-1}, {"c",1e-2},
        {"m",1e-3}, {"µ",1e-6}, {"n",1e-9}, {"p",1e-12},{"f",1e-15},
        {"a",1e-18},{"z",1e-21},{"y",1e-24},{"r",1e-27},{"q",1e-30}
    };
    return table;
}

inline const std::map<std::string,std::string>& SI_UNITS() {
    static std::map<std::string,std::string> table = {
        {"L","m"},
        {"M","kg"},
        {"T","s"},
        {"I","A"},
        {"Θ","K"},
        {"N","mol"},
        {"J","cd"}
    };
    return table;
}

inline const std::unordered_map<std::string,UnitInfo>& UNIT_TABLE() {
    static std::unordered_map<std::string,UnitInfo> table = {
        // Base units
        {"m", {1.0,   {{"L",1}} }},                                       // Metre
        {"kg",{1.0,   {{"M",1}} }},                                       // Kilogram
        {"g", {1e-3,  {{"M",1}} }},                                       // Gram
        {"s", {1.0,   {{"T",1}} }},                                       // Second

        // Derived SI
        {"N", {1.0,   {{"M",1},{"L",1},{"T",-2}} }},                // Newton
        {"J", {1.0,   {{"M",1},{"L",2},{"T",-2}} }},                // Joule
        {"W", {1.0,   {{"M",1},{"L",2},{"T",-3}} }},                // Watt

        // Other
        {"eV",{Globals::Constant::eV,{{"M",1},{"L",2},{"T",-2}} }}, // electronvolt
    };
    return table;
}

inline UnitInfo parseUnit(const std::string& unit) {
    if (const auto it = UNIT_TABLE().find(unit); it != UNIT_TABLE().end()) return it->second;

    // Sort units such that multi characters are checked first. Handles clashes for da and d but kept general
    static const std::vector<std::pair<std::string, double>> sorted_prefixes = [] {
        std::vector<std::pair<std::string, double>> units(PREFIXES().begin(), PREFIXES().end());
        std::ranges::sort(units, [](const auto& a, const auto& b) {
            return a.first.size() > b.first.size(); // longest prefix first
        });
        return units;
    }();

    // prefix is the unit and factor is the corresponding value (See prefixes table)
    for (const auto& [prefix, factor] : sorted_prefixes) {
        if (unit.rfind(prefix, 0) == 0) { // Look at the start of the string
            std::string base = unit.substr(prefix.size()); // Remove units (e.g. km -> m or dam -> m)
            if (const auto base_it = UNIT_TABLE().find(base); base_it != UNIT_TABLE().end()) {
                UnitInfo unitInfo = base_it->second;
                unitInfo.scale *= factor; // Multiply by scale (for units like eV) by factor to get multiplier on SI units
                return unitInfo;
            }
            // Base unit not found → throw an error
            throw std::invalid_argument(
                std::format("Unknown base unit: {} (after prefix '{}')", base, prefix)
                );
        }
    }
    throw std::invalid_argument(std::format("Unknown unit: {}", unit));
}

inline UnitInfo parseUnits(const std::string& units) {
    UnitInfo result{1.0, Dimension::dimensionless()};
    for (const auto& token : splitTerms(units)) {
        if (token.empty()) continue;
        const int exponent = parseExponent(token);
        std::string base = stripExponent(token);
        UnitInfo unitInfo = parseUnit(base);
        multiplyUnit(result, unitInfo, exponent);
    }
    return result;
}

#endif //PHYSICS_SIMULATION_PROGRAM_UNITS_H