//
// Created by Tobias Sharman on 11/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H
#define PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H

#include "units.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <stdexcept>

class Utf8Scanner {
public:
    explicit Utf8Scanner(const std::string_view s) : data(s) {}

    [[nodiscard]] bool eof() const { return pos >= data.size(); }
    [[nodiscard]] size_t position() const { return pos; }

    [[nodiscard]] unsigned char peekByte() const {
        return eof() ? 0 : static_cast<unsigned char>(data[pos]);
    }

    void advance(const size_t n = 1) { pos += n; }

    [[nodiscard]] bool matchSequence(const std::initializer_list<unsigned char> sequence) const {
        if (pos + sequence.size() > data.size()) return false;
        size_t i = 0;
        for (const unsigned char expected : sequence) {
            if (static_cast<unsigned char>(data[pos + i]) != expected) return false;
            i++;
        }
        return true;
    }

private:
    std::string_view data;
    size_t pos{0};
};

inline std::pair<std::string,double> extractPrefix(const std::string_view unit) {
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

inline int decodeSuperscript(const std::string& s) {
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

#endif //PHYSICS_SIMULATION_PROGRAM_UNIT_UTILITIES_H