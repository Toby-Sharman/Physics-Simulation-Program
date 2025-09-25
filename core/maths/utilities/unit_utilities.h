//
// Physics-Simulation-program
// File: unit_utilities.cpp
// Created by Tobias Sharman on 11/09/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

// TODO: reformat with format library

#ifndef PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H
#define PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H

#include "units.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <stdexcept>

// Scanner to handle UTF-8 encoded input
class Utf8Scanner {
    public:
        explicit Utf8Scanner(std::string_view s);

        [[nodiscard]] bool eof() const;
        [[nodiscard]] size_t position() const;

        [[nodiscard]] unsigned char peekByte() const;
        void advance(size_t n = 1);

        [[nodiscard]] bool matchSequence(std::initializer_list<unsigned char> sequence) const;

    private:
        std::string_view data;
        size_t pos{0};
};

// Utility functions for units
std::pair<std::string,double> extractPrefix(std::string_view unit);
int decodeSuperscript(const std::string& s);
UnitInfo parseUnit(const std::string& token);
UnitInfo parseUnits(const std::string& expr);

#endif // PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H
