//
// Physics-Simulation-program
// File: unit_utilities.cpp
// Created by Tobias Sharman on 25/09/2025
//
// Implementation of unit_utilities.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include <cctype> // Ignore IDE warning of possibly unused it is used for std::isalpha and std::isspace
#include <charconv>
#include <cmath>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include "core/maths/utilities/unit_utilities.h"
#include "core/maths/utilities/units.h"

std::pair<std::string_view, int> extractSuperscript(const std::string_view unit) noexcept {
    if (unit.empty()) { return {unit, 0}; } // If empty return input with exponent = 0

    const char* start = unit.data();
    const char* end = start + unit.size();
    const char* p = start;

    // Find the start of the exponent
    while (p != end) {
        const char c = *p;
        if ((c >= '0' && c <= '9') || c == '-') {
            break; // Exponent starts here
        }
        if (c == '^') {
            ++p; // Skip caret
            break; // Exponent starts after caret
        }
        ++p;
    }

    // Number parsing
    if (const char* numStart = p; numStart != end) {
        int exponent = 0;
        if (auto [ptr, ec] = std::from_chars(numStart, end, exponent); ec == std::errc()) {
            const char* baseEnd = (numStart > start && *(numStart - 1) == '^') ? numStart - 1 : numStart;
            return {std::string_view(start, baseEnd - start), exponent};
        }
    }

    return {unit, 1}; // No numeric exponent found
}

UnitInfo extractPrefix(const std::string_view unit) {
    auto& table = unitTable(); // Load unit table

    // Search for prefix, unit combination
    for (const auto &[symbol, scale] : prefixes) {
        if (const auto prefix = symbol; unit.substr(0, prefix.size()) == prefix) {
            if (const auto it = table.find(unit.substr(prefix.size())); it != table.end()) {
                return {it->second.factor * scale, it->second.unit};
            }
        }
    }

    // Search for standalone unit
    if (const auto it = table.find(unit); it != table.end()) {
        return it->second;
    }

    // Throw error for if no valid unit was found
    throw std::invalid_argument(
        std::format(
            "Could not find a valid prefix, base unit combination or stand alone base unit from prefixes and unit tables for {}",
            unit
        )
    );
}

UnitInfo parseUnit(const char* tokenStart, const char* tokenEnd) {
    // Trim leading spaces
    while (tokenStart < tokenEnd && std::isspace(static_cast<unsigned char>(*tokenStart))) {
        ++tokenStart;
    }

    // Trim trailing spaces
    while (tokenEnd > tokenStart && std::isspace(static_cast<unsigned char>(*(tokenEnd - 1)))) {
        --tokenEnd;
    }

    if (tokenStart == tokenEnd) {
        // Only spaces / empty
        return {1.0, Unit::dimensionless()};
    }

    std::string cleanedToken;
    cleanedToken.reserve(tokenEnd - tokenStart);

    // Copy while skipping internal spaces
    for (const char* p = tokenStart; p < tokenEnd; ++p) {
        if (!std::isspace(static_cast<unsigned char>(*p))) {
            cleanedToken.push_back(*p);
        }
    }

    auto [baseUnit, exponent] = extractSuperscript(cleanedToken); // baseUnit = string view, exponent = int
    auto [scale, siUnit] = extractPrefix(baseUnit); // scale = double, siUnit = 7d array of int8_t
    return {std::pow(scale, exponent), siUnit.raisedTo(exponent)};
}

UnitInfo parseUnits(const std::string_view units) {
    UnitInfo result = {1.0, Unit::dimensionless()};

    const char* p = units.data();
    const char* end = p + units.size();

    bool divideNext = false; // false = multiply, true = divide

    while (p < end) {
        // Skip leading whitespace
        while (p < end && std::isspace(static_cast<unsigned char>(*p))) ++p;
        if (p == end) break;

        // Explicit operators
        if (*p == '*') { divideNext = false; ++p; continue; } // Not necessary but added protection
        if (*p == '/') { divideNext = true;  ++p; continue; }

        // Capture token until next '*' or '/' or space
        const char* tokenStart = p;
        while (p < end && *p != '*' && *p != '/' && !std::isspace(static_cast<unsigned char>(*p))) ++p;
        if (std::isspace(static_cast<unsigned char>(*p))) {
            // Handling for spaces stuff like '^' operators clashing with spaces representing implicit multiplication
            while (p < end && std::isspace(static_cast<unsigned char>(*p))) ++p;
            while (p < end && *p != '*' && *p != '/' // Operators
                && !std::isalpha(static_cast<unsigned char>(*p)) // Letters
                && !((static_cast<unsigned char>(*p) == 0xCE) && (p + 1 < end && static_cast<unsigned char>(p[1]) == 0xBC))) { // μ for UTF-8 encoding
                ++p;
                }
            --p; // Since pointer is on something indicative of next token or a multiplicative operator
        }

        const char* tokenEnd = p;

        // Clean token of any spaces and parse it
        auto [factor, siUnit] = parseUnit(tokenStart, tokenEnd);

        // Adding member functions for operations may reduce any temporary creations in updating result
        if (divideNext) {
            result = {result.factor / factor, result.unit / siUnit};
        } else {
            result = {result.factor * factor, result.unit * siUnit};
        }

        divideNext = false; // Reset so whitespace between units implies multiply; if not '/' multiply
    }
    return result;
}