//
// Created by Tobias Sharman on 10/09/2025.
//

// Exponent handling is limited to integers otherwise handling is spotty

#ifndef PHYSICS_SIMULATION_PROGRAM_UNITS_H
#define PHYSICS_SIMULATION_PROGRAM_UNITS_H

#include <string>
#include <unordered_map>
#include <array>
#include <initializer_list>
#include <map>

struct Dimension {
    std::map<std::string,int> exponents;

    Dimension() = default;
    Dimension(const std::initializer_list<std::pair<const std::string,int>> init)
        : exponents(init) {}

    static Dimension dimensionless() { return {}; }

    bool operator==(const Dimension& other) const { return exponents == other.exponents; }
    bool operator!=(const Dimension& other) const { return !(*this == other); }

    Dimension operator+(const Dimension& other) const {
        Dimension res = *this;
        for (auto& [k,v] : other.exponents) res.exponents[k] += v;
        return res.removeEmpty();
    }

    Dimension operator-(const Dimension& other) const {
        Dimension res = *this;
        for (auto& [k,v] : other.exponents) res.exponents[k] -= v;
        return res.removeEmpty();
    }

    Dimension& removeEmpty() {
        for (auto it = exponents.begin(); it != exponents.end();) {
            it = (it->second == 0) ? exponents.erase(it) : std::next(it);
        }
        return *this;
    }

    [[nodiscard]] std::string toString() const {
        std::string out;
        for (auto& [k,v] : exponents) {
            out += k + "^" + std::to_string(v) + " ";
        }
        if (!out.empty() && out.back() == ' ') out.pop_back();
        return out.empty() ? "dimensionless" : out;
    }
};

struct UnitInfo {
    double scale;
    Dimension dimension;
};

struct Prefix {
    std::string_view symbol;
    double scale;
};

inline const std::array<Prefix, 24>& PREFIXES() {
    static const std::array<Prefix, 24> table = {{
        {"da", 1e1},   {"h", 1e2},  {"k", 1e3},
        {"M",  1e6},   {"G", 1e9},  {"T", 1e12},
        {"P",  1e15},  {"E", 1e18}, {"Z", 1e21},
        {"Y",  1e24},  {"R", 1e27}, {"Q", 1e30},

        {"d", 1e-1},  {"c", 1e-2},  {"m", 1e-3},
        {"µ", 1e-6},  {"n", 1e-9},  {"p", 1e-12},
        {"f", 1e-15}, {"a", 1e-18}, {"z", 1e-21},
        {"y", 1e-24}, {"r", 1e-27}, {"q", 1e-30}
    }};
    return table;
}

inline const std::unordered_map<std::string_view,UnitInfo>& UNIT_TABLE() {
    static const std::unordered_map<std::string_view,UnitInfo> table = { // Sorted in L, M, T, I, Θ, N, J order
        // Base units
        {"m",  {1.0,  {{"L",1} }} },   // metre
        {"kg", {1.0,  {{"M",1} }} },   // kilogram
        {"g",  {1e-3, {{"M",1} }} },   // gram
        {"s",  {1.0,  {{"T",1} }} },   // second
        {"A",  {1.0,  {{"I",1} }} },   // ampere
        {"K",  {1.0,  {{"Θ",1} }} },   // kelvin
        {"mol",{1.0,  {{"N",1} }} },   // mole
        {"cd", {1.0,  {{"J",1} }} },   // candela

        // Derived SI
        {"Hz",  {1.0, {{"T",-1} }} },                                     // hertz
        {"N",   {1.0, {{"M",1}, {"L",1}, {"T",-2} }} },             // newton
        {"Pa",  {1.0, {{"M",1}, {"L",-1},{"T",-2} }} },             // pascal
        {"J",   {1.0, {{"M",1}, {"L",2}, {"T",-2} }} },             // joule
        {"W",   {1.0, {{"M",1}, {"L",2}, {"T",-3} }} },             // watt
        {"C",   {1.0, {{"T",1}, {"I",1} }} },                          // coulomb
        {"V",   {1.0, {{"M",1}, {"L",2}, {"T",-3},{"I",-1} }} }, // volt
        {"F",   {1.0, {{"M",-1},{"L",-2},{"T",4}, {"I",2} }} },  // farad
        {"Ω",   {1.0, {{"M",1}, {"L",2}, {"T",-3},{"I",-2} }} }, // ohm
        {"S",   {1.0, {{"M",-1},{"L",-2},{"T",3}, {"I",2} }} },  // siemens
        {"Wb",  {1.0, {{"M",1}, {"L",2}, {"T",-2},{"I",-1} }} }, // weber
        {"T",   {1.0, {{"M",1}, {"T",-2},{"I",-1} }} },             // tesla
        {"H",   {1.0, {{"M",1}, {"L",2}, {"T",-2},{"I",-2} }} }, // henry
        // {"Bq",  {1.0, {{"T",-1} }} },                                     // becquerel
        // {"Gy",  {1.0, {{"L",2}, {"T",-2} }} },                         // gray
        // {"Sv",  {1.0, {{"L",2}, {"T",-2} }} },                         // sievert
        // {"kat", {1.0, {{"N",1}, {"T",-1} }} }                          // katal
    };
    return table;
}

#endif //PHYSICS_SIMULATION_PROGRAM_UNITS_H