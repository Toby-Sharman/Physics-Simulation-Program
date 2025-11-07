//
// Physics Simulation Program
// File: units.h
// Created by Tobias Sharman on 16/09/2025
//
// Description:
//   - Describes Unit custom data type and sets up tables for prefixes and units
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_UNITS_H
#define PHYSICS_SIMULATION_PROGRAM_UNITS_H

#include <array>
#include <cmath>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/globals.h"

// Prefix
//
// Represents the different prefixes for SI units, where prefixes represent a shorthand for orders of magnitude
//   - symbol is the symbol of that prefix
//   - scale is the amount that prefix represents
struct Prefix {
    std::string_view symbol;
    double scale;
};

// Table of prefixes for all standard prefixes
inline constexpr std::array<Prefix, 24> prefixes = {{
    {"da", 1e1},   {"h", 1e2},  {"k", 1e3},
    {"M",  1e6},   {"G", 1e9},  {"T", 1e12},
    {"P",  1e15},  {"E", 1e18}, {"Z", 1e21},
    {"Y",  1e24},  {"R", 1e27}, {"Q", 1e30},

    {"d", 1e-1},  {"c", 1e-2},  {"m", 1e-3},
    {"µ", 1e-6},  {"n", 1e-9},  {"p", 1e-12},
    {"f", 1e-15}, {"a", 1e-18}, {"z", 1e-21},
    {"y", 1e-24}, {"r", 1e-27}, {"q", 1e-30}
}};

// Unit
//
// Represents the dimensional exponents of a physical quantity according to the seven base SI dimensions:
//   L = length
//   M = mass
//   T = time
//   I = electric current
//   Θ = thermodynamic temperature
//   N = amount of substance
//   J = luminous intensity
// as int8_ts in an array, where the value of each index is the exponent on that dimension.
//
// Notes on initialisation:
//   - Construct via curly or standard braces
//
// Notes on algorithms:
//   - Each exponent is stored as an int8_t in a fixed-size array of length 7
//         -> Usage of int8_t limits the allowed values of exponents but will give some performance increase
//         -> No sensible quantity should be exceeding [-128, 127]
//   - Some functions may seem overly verbose, but this is intentional to stop any, however minor, performance hit from
//     debug modes
//         -> Compiler would normally unpack small for loops that may be used for the operator overloads
//         -> For assignment operators there is repeat to avoid temporaries
//   - Equality and inequality are branchless since short-circuiting should not be faster for Quantity use cases
//   - The casting warning from Clang-Tidy or otherwise is not a problem for this use case and the value will not
//     propagate in a way that would cause issues in other places
//         -> If wanting to get rid of warning static_cast<int16_t> or static_cast<short> will work
//
// Notes on output:
//   - The toString() method outputs each non-zero exponent in the form SYMBOL^EXPONENT (omitting ^1 for brevity),
//     separated by spaces
//         -> Outputted in base dimensions (L, M, T, I, Θ, N, J)
//         -> If all exponents are zero, it returns "dimensionless"
//         -> toString is for testing not for nice outputs
//
// Supported overloads / operations and functions / methods:
//   - Constructor:            Unit()
//   - Dimensionless factory:  Unit::dimensionless()
//   - Multiplication:         operator*, operator*=
//   - Division:               operator/, operator/=
//   - Equality/Inequality:    operator==, operator!=
//   - Less-than:              operator<
//   - Exponentiation:         raisedTo(n)
//   - Inverse:                inverse()
//   - String conversion:      toString()
//
// Example usage:
//   Unit meter = Unit(1, 0, 0, 0, 0, 0, 0);
//   Unit second = Unit(0, 0, 1, 0, 0, 0, 0);
//
//   Unit speed = meter / second;           // {1,0,-1,0,0,0,0}
//   Unit area  = meter * meter;            // {2,0,0,0,0,0,0}
//   Unit volume = meter.raisedTo(3);       // {3,0,0,0,0,0,0}
//   Unit perSecond = second.inverse();     // {0,0,-1,0,0,0,0}
//   Unit acceleration = meter;
//   acceleration /= second.raisedTo(2);    // {1,0,-2,0,0,0,0}
//
//   bool same = (speed == meter / second); // true
//
//   std::cout << "Speed: " << speed.toString() << "\n"; // Output: "L T^-1"
struct [[nodiscard]] Unit {
    std::array<std::int8_t, 7> exponents; // L, M, T, I, Θ, N, J

    // Default Constructor
    constexpr Unit() noexcept : exponents{0,0,0,0,0,0,0} {}

    // Dimensionless Unit factory
    //
    // Returns a Unit with all exponents zero
    // Used to make it clearer when there is dimensionless Unit initialisation
    [[nodiscard]] static constexpr Unit dimensionless() noexcept {
        return Unit{0,0,0,0,0,0,0};
    }

    // Constructor method from array
    explicit constexpr Unit(const std::array<std::int8_t, 7> exps) noexcept : exponents(exps) {}

    // Constructor method from values
    constexpr Unit(
        const std::int8_t l,
        const std::int8_t m,
        const std::int8_t t,
        const std::int8_t i,
        const std::int8_t theta,
        const std::int8_t n,
        const std::int8_t j
    ) noexcept
        : exponents{l, m, t, i, theta, n, j} {}

    // Multiplication operator
    //
    // Multiplies two Units by adding their corresponding exponents
    [[nodiscard]] constexpr Unit operator*(const Unit& other) const noexcept {
        return Unit{
            static_cast<std::int8_t>(this->exponents[0] + other.exponents[0]),
            static_cast<std::int8_t>(this->exponents[1] + other.exponents[1]),
            static_cast<std::int8_t>(this->exponents[2] + other.exponents[2]),
            static_cast<std::int8_t>(this->exponents[3] + other.exponents[3]),
            static_cast<std::int8_t>(this->exponents[4] + other.exponents[4]),
            static_cast<std::int8_t>(this->exponents[5] + other.exponents[5]),
            static_cast<std::int8_t>(this->exponents[6] + other.exponents[6])
        };
    }

    // Multiplication assignment operator
    //
    // Adds the exponents of another Unit to this Unit in-place
    constexpr Unit& operator*=(const Unit& other) noexcept {
        this->exponents[0] = static_cast<std::int8_t>(this->exponents[0] + other.exponents[0]);
        this->exponents[1] = static_cast<std::int8_t>(this->exponents[1] + other.exponents[1]);
        this->exponents[2] = static_cast<std::int8_t>(this->exponents[2] + other.exponents[2]);
        this->exponents[3] = static_cast<std::int8_t>(this->exponents[3] + other.exponents[3]);
        this->exponents[4] = static_cast<std::int8_t>(this->exponents[4] + other.exponents[4]);
        this->exponents[5] = static_cast<std::int8_t>(this->exponents[5] + other.exponents[5]);
        this->exponents[6] = static_cast<std::int8_t>(this->exponents[6] + other.exponents[6]);
        return *this;
    }

    // Division operator
    //
    // Divides two Units by subtracting the exponents of the right-hand side from the left-hand side
    [[nodiscard]] constexpr Unit operator/(const Unit& other) const noexcept {
        return Unit{
            static_cast<std::int8_t>(this->exponents[0] - other.exponents[0]),
            static_cast<std::int8_t>(this->exponents[1] - other.exponents[1]),
            static_cast<std::int8_t>(this->exponents[2] - other.exponents[2]),
            static_cast<std::int8_t>(this->exponents[3] - other.exponents[3]),
            static_cast<std::int8_t>(this->exponents[4] - other.exponents[4]),
            static_cast<std::int8_t>(this->exponents[5] - other.exponents[5]),
            static_cast<std::int8_t>(this->exponents[6] - other.exponents[6])
        };
    }

    // Division assignment operator
    //
    // Subtracts the exponents of another Unit from this Unit in-place
    constexpr Unit& operator/=(const Unit& other) noexcept {
        this->exponents[0] = static_cast<std::int8_t>(this->exponents[0] - other.exponents[0]);
        this->exponents[1] = static_cast<std::int8_t>(this->exponents[1] - other.exponents[1]);
        this->exponents[2] = static_cast<std::int8_t>(this->exponents[2] - other.exponents[2]);
        this->exponents[3] = static_cast<std::int8_t>(this->exponents[3] - other.exponents[3]);
        this->exponents[4] = static_cast<std::int8_t>(this->exponents[4] - other.exponents[4]);
        this->exponents[5] = static_cast<std::int8_t>(this->exponents[5] - other.exponents[5]);
        this->exponents[6] = static_cast<std::int8_t>(this->exponents[6] - other.exponents[6]);
        return *this;
    }

    // Equality operator
    //
    // Compares all exponents for exact equality
    [[nodiscard]] constexpr bool operator==(const Unit& other) const noexcept {
        return this->exponents == other.exponents;
    }

    // Inequality operator
    //
    // Compares all exponents for inequality
    [[nodiscard]] constexpr bool operator!=(const Unit& other) const noexcept {
        return this->exponents != other.exponents;
    }

    // Less-than operator
    //
    // Compares all exponents lexicographically according to std::array's operator <
    [[nodiscard]] constexpr bool operator<(const Unit& other) const noexcept {
        return exponents < other.exponents;
    }

    // Exponentiation method
    //
    // Raises a Unit to any real power and throws an error if the resultant exponent is non-integer
    [[nodiscard]] Unit raisedTo(const double power) const {
        Unit result = {0, 0, 0, 0, 0, 0, 0};
        for (std::size_t i = 0; i < 7; ++i) {
            const double newExponent = static_cast<double>(exponents[i]) * power;
            if (std::round(newExponent) != newExponent) {
                std::ostringstream oss;
                oss << "Cannot raise unit '" << toString() << "' to power " << power
                    << " because exponent " << i << " becomes non-integer (" << newExponent << ")";
                throw std::domain_error(oss.str());
            }

            result.exponents[i] = static_cast<std::int8_t>(std::round(newExponent));
        }

        return result;
    }

    // Inverse method
    //
    // Negates all exponents, equivalent to raising the Unit to the power -1
    [[nodiscard]] constexpr Unit inverse() const noexcept {
        return Unit{
            static_cast<std::int8_t>(-this->exponents[0]),
            static_cast<std::int8_t>(-this->exponents[1]),
            static_cast<std::int8_t>(-this->exponents[2]),
            static_cast<std::int8_t>(-this->exponents[3]),
            static_cast<std::int8_t>(-this->exponents[4]),
            static_cast<std::int8_t>(-this->exponents[5]),
            static_cast<std::int8_t>(-this->exponents[6])
        };
    }

    // Convert to string method
    //
    // Non-zero exponents are appended in the form SYMBOL^EXPONENT (exponent 1 is omitted), separated by spaces
    // Returns "Dimensionless" if all exponents are zero
    [[nodiscard]] constexpr std::string toString() const {
        static const std::array<std::string,7> symbols = {"L","M","T","I","Θ","N","J"};
        std::string s;
        s.reserve(32); // Rough estimate to reduce reallocations

        for (std::size_t i = 0; i < symbols.size(); ++i) {
            if (const int exponent = this->exponents[i]; exponent != 0) { // Ignore warning there is no issue with usage here
                s.append(symbols[i]);
                if (exponent != 1) {
                    s.append("^" + std::to_string(exponent));
                }
                s.push_back(' ');
            }
        }

        if (s.empty()) {
            return "Dimensionless";
        }

        s.pop_back(); // remove trailing space
        return s;
    }
};

// UnitInfo
//
// Represents the details of an SI unit
//   - factor is the amount that needs to be multiplied by to get to SI
//   - Unit is the dimensions
struct UnitInfo {
    double factor;
    Unit unit;
};

// Table of base SI units
//
// Replacing kg with g for easier use in algorithms by keeping unitTable prefix free. With a 1e-3 factor to keep with kg
// as the base SI unit
//
// SI dimensionless units like radians and degrees are neglected
//   - Unsure of implication on lumen and lux FIXME
[[nodiscard]] inline const std::unordered_map<std::string_view, UnitInfo>& unitTable() {
    using PhysConst = Globals::Constant::Physics;
    static const std::unordered_map<std::string_view, UnitInfo> table = {
        // Base SI units (except kg -> g)
        {"m",   {1.0,  Unit(1,  0,  0,  0,  0, 0, 0)}}, // Metre
        {"g",   {1e-3, Unit(0,  1,  0,  0,  0, 0, 0)}}, // Gram
        {"s",   {1.0,  Unit(0,  0,  1,  0,  0, 0, 0)}}, // Second
        {"A",   {1.0,  Unit(0,  0,  0,  1,  0, 0, 0)}}, // Ampere
        {"K",   {1.0,  Unit(0,  0,  0,  0,  1, 0, 0)}}, // Kelvin
        {"mol", {1.0,  Unit(0,  0,  0,  0,  0, 1, 0)}}, // Mole
        {"cd",  {1.0,  Unit(0,  0,  0,  0,  0, 0, 1)}}, // Candela

        // Derived SI units
        {"Hz",  {1.0,  Unit(0,  0,  -1, 0,  0, 0, 0)}}, // Hertz
        {"N",   {1.0,  Unit(1,  1,  -2, 0,  0, 0, 0)}}, // Newton
        {"Pa",  {1.0,  Unit(-1, 1,  -2, 0,  0, 0, 0)}}, // Pascal
        {"J",   {1.0,  Unit(2,  1,  -2, 0,  0, 0, 0)}}, // Joule
        {"W",   {1.0,  Unit(2,  1,  -3, 0,  0, 0, 0)}}, // Watt
        {"C",   {1.0,  Unit(0,  0,  1,  1,  0, 0, 0)}}, // Coulomb
        {"V",   {1.0,  Unit(2,  1,  -3, -1, 0, 0, 0)}}, // Volt
        {"F",   {1.0,  Unit(-2, -1, 4,  2,  0, 0, 0)}}, // Farad
        {"Ω",   {1.0,  Unit(2,  1,  -3, -2, 0, 0, 0)}}, // Ohm
        {"S",   {1.0,  Unit(-2, -1, 3,  2,  0, 0, 0)}}, // Siemens
        {"Wb",  {1.0,  Unit(2,  1,  -2, -1, 0, 0, 0)}}, // Weber
        {"T",   {1.0,  Unit(0,  1,  -2, -1, 0, 0, 0)}}, // Tesla
        {"H",   {1.0,  Unit(2,  1,  -2, -2, 0, 0, 0)}}, // Henry
        {"lm",  {1.0,  Unit(0,  0,  0,  0,  0, 0, 1)}}, // Lumen
        {"lx",  {1.0,  Unit(-2, 0,  0,  0,  0, 0, 1)}}, // Lux
        {"Bq",  {1.0,  Unit(0,  0,  -1, 0,  0, 0, 0)}}, // Becquerel
        {"Gy",  {1.0,  Unit(2,  0,  -2, 0,  0, 0, 0)}}, // Gray
        {"Sv",  {1.0,  Unit(2,  0,  -2, 0,  0, 0, 0)}}, // Sievert
        {"kat", {1.0,  Unit(0,  0,  -1, 0,  0, 1, 0)}}, // Katal

        // Non-SI units
        {"eV",   {PhysConst::e,    Unit(2, 1, -2, 0,  0, 0, 0)}}, // ElectronVolt
        {"u",    {PhysConst::u,    Unit(0, 1, 0,  0,  0, 0, 0)}}, // Atomic mass unit
        {"Da",   {PhysConst::Da,   Unit(0, 1, 0,  0,  0, 0, 0)}}, // Dalton

        {"min", {60.0,   Unit(0, 0, 1, 0, 0, 0, 0)}}, // Minute
        {"hr",  {3600.0, Unit(0, 0, 1, 0, 0, 0, 0)}}, // Hour
    };
    return table;
}

#endif //PHYSICS_SIMULATION_PROGRAM_UNITS_H