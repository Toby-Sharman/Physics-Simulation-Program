//
// Physics-Simulation-program
// File: unit_utilities.cpp
// Created by Tobias Sharman on 25/09/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "core/maths/utilities/unit_utilities.h"

Utf8Scanner::Utf8Scanner(const std::string_view s) : data(s) {}

bool Utf8Scanner::eof() const { return pos >= data.size(); }
size_t Utf8Scanner::position() const { return pos; }

unsigned char Utf8Scanner::peekByte() const {
    return eof() ? 0 : static_cast<unsigned char>(data[pos]);
}

void Utf8Scanner::advance(const size_t n) { pos += n; }

bool Utf8Scanner::matchSequence(const std::initializer_list<unsigned char> sequence) const {
    if (pos + sequence.size() > data.size()) return false;
    size_t i = 0;
    for (const unsigned char expected : sequence) {
        if (static_cast<unsigned char>(data[pos + i]) != expected) return false;
        i++;
    }
    return true;
}

std::pair<std::string,double> extractPrefix(const std::string_view unit) {
    const auto& prefixes = PREFIXES();
    const auto& units = UNIT_TABLE();

    std::string bestBase;
    double bestScale = 1.0;
    size_t bestLen = 0;

    for (const auto&[symbol, scale] : prefixes) { // TODO: Collapse if statements
        if (unit.substr(0, symbol.size()) == symbol) {
            auto base = std::string(unit.substr(symbol.size()));
            if (!base.empty() && units.contains(base)) {
                if (symbol.size() > bestLen) {
                    bestBase = base;
                    bestScale = scale;
                    bestLen = symbol.size();
                }
            }
        }
    }

    if (bestLen > 0) {
        return { bestBase, bestScale };
    }

    // No prefix match → return unchanged
    return { std::string(unit), 1.0 };
}

int decodeSuperscript(const std::string& s) {
    static const std::unordered_map<std::string_view,int> supers = {
        {"⁰", 0}, {"¹", 1}, {"²", 2}, {"³", 3}, {"⁴", 4},
        {"⁵", 5}, {"⁶", 6}, {"⁷", 7}, {"⁸", 8}, {"⁹", 9}
    };

    Utf8Scanner scan(s);
    int value = 0;
    bool negative = false;

    while (!scan.eof()) {
        // Superscript minus (⁻ = E2 81 BB)
        if (scan.matchSequence({0xE2, 0x81, 0xBB})) {
            negative = true;
            scan.advance(3);
            continue;
        }

        // Collect one UTF-8 char
        std::string utf8Char;
        utf8Char.push_back(static_cast<char>(scan.peekByte()));
        scan.advance();
        while (!scan.eof() && (scan.peekByte() & 0xC0) == 0x80) {
            utf8Char.push_back(static_cast<char>(scan.peekByte()));
            scan.advance();
        }

        auto it = supers.find(utf8Char);
        if (it == supers.end()) {
            throw std::runtime_error("Invalid superscript sequence: " + utf8Char);
        }
        value = value * 10 + it->second;
    }

    return negative ? -value : value;
}

// Parse a single unit token (with optional prefix + exponent)
UnitInfo parseUnit(const std::string& token) {
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
UnitInfo parseUnits(const std::string& expr) {
    UnitInfo result{1.0, Dimension::dimensionless()};
    std::string token;
    char op = '*'; // operator for the current token

    auto processToken = [&](const std::string& token_, const char op_) {
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
                processToken(token, op);
                token.clear();
            }
            scan.advance();
        }
        else if (c == '*' || c == '/') {
            processToken(token, op);
            token.clear();
            op = (c == '/') ? '/' : '*';
            scan.advance();
        }
        else if (scan.matchSequence({0xC2, 0xB7})) { // middle dot
            processToken(token, op);
            token.clear();
            op = '*';
            scan.advance(2);
        }
        else {
            token.push_back(static_cast<char>(c));
            scan.advance();
        }
    }

    processToken(token, op);
    return result;
}