//
// Created by Tobias Sharman on 22/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_UNIT_PARSING_H
#define PHYSICS_SIMULATION_PROGRAM_UNIT_PARSING_H

#include <string>

#include "units.h"
#include "unit_utilities.h"

// Parse a single unit token (with optional prefix + exponent)
inline UnitInfo parseUnit(const std::string& token) {
    std::string unit;
    int exponent = 1;

    // Caret (^) form
    if (const size_t caretPos = token.find('^'); caretPos != std::string::npos) {
        unit = token.substr(0, caretPos);
        exponent = std::stoi(token.substr(caretPos + 1));
    } else {
        // Scan manually
        Utf8Scanner scan(token);
        size_t split = token.size();

        while (!scan.eof()) {
            const unsigned char c = scan.peekByte();
            if (std::isdigit(c) || c == '-') {
                split = scan.position();
                break;
            }
            if ((c & 0xF0) == 0xE0) { // superscript
                split = scan.position();
                break;
            }
            scan.advance();
        }

        unit = token.substr(0, split);

        if (split < token.size()) {
            if (const std::string exponentPart = token.substr(split);
                !exponentPart.empty() && (std::isdigit(exponentPart[0]) || exponentPart[0] == '-'))
                exponent = std::stoi(exponentPart);
            else
                exponent = decodeSuperscript(exponentPart);
        }
    }

    // Prefix handling
    auto [baseUnit, prefixScale] = extractPrefix(unit);

    if (baseUnit.empty()) {
        throw std::runtime_error("Parsed an empty base unit from token: '" + token + "'");
    }

    // Lookup
    const auto& table = UNIT_TABLE();
    const auto it = table.find(baseUnit);
    if (it == table.end()) throw std::runtime_error("Unknown unit: " + baseUnit);

    const auto&[scale_, dimension_] = it->second;
    const double totalScale = std::pow(prefixScale * scale_, exponent);

    Dimension dimension;
    for (auto& [k,v] : dimension_.exponents) {
        dimension.exponents[k] += v * exponent;
    }

    return UnitInfo{totalScale, dimension};
}

// Parse a compound unit string (spaces, *, /, · separators)
inline UnitInfo parseUnits(const std::string& expr) {
    UnitInfo result{1.0, Dimension::dimensionless()};
    std::string token;
    char op = '*'; // operator for the current token

    auto flushToken = [&](const std::string& token_, const char op_) {
        if (token_.empty()) return;

        auto [scale, dimension] = parseUnit(token_);

        if (op_ == '*') {
            result.dimension = result.dimension + dimension;
            result.scale *= scale;
        } else if (op_ == '/') {
            result.dimension = result.dimension - dimension;
            result.scale /= scale;
        }
    };

    Utf8Scanner scan(expr);
    while (!scan.eof()) {
        if (const unsigned char c = scan.peekByte(); c == ' ') {
            if (!token.empty()) {
                flushToken(token, op);
                token.clear();
            }
            scan.advance();
        }
        else if (c == '*' || c == '/') {
            flushToken(token, op);
            token.clear();
            op = (c == '/') ? '/' : '*';
            scan.advance();
        }
        else if (scan.matchSequence({0xC2, 0xB7})) { // middle dot
            flushToken(token, op);
            token.clear();
            op = '*';
            scan.advance(2);
        }
        else {
            token.push_back(static_cast<char>(c));
            scan.advance();
        }
    }

    flushToken(token, op);
    return result;
}

#endif //PHYSICS_SIMULATION_PROGRAM_UNIT_PARSING_H