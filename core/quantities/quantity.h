//
// Physics Simulation Program
// File: quantity.h
// Created by Tobias Sharman on 16/09/2025
//
// Description:
//   - Describes Quantity custom data type and sets up a table for some physical quantities
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_QUANTITY_H
#define PHYSICS_SIMULATION_PROGRAM_QUANTITY_H

#include <cmath>
#include <format>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

#include "units.h"
#include "core/quantities//utilities/unit_utilities.h"

// Quantity
//
// Represents a physical quantity with a numeric value and a dimensional unit
//   - The unit is represented using the Unit class, which tracks powers of the seven base SI dimensions
//
// This class supports arithmetic operations, dimension checking, and printing
//
// Notes on initialisation:
//   - Can be constructed with value and unit arguments
//         -> Value is taken as a double
//         -> Unit can be either passed as a Unit type or a string that will be parsed into a Unit type by parseUnits
//
// Notes on algorithms:
//   - Addition and subtraction require identical units (dimensions must match exactly)
//   - Multiplication and division automatically combine units via Unit operators
//
// Notes on output:
//   - The unit is printed using the base dimension symbols (L, M, T, I, Θ, N, J)
//         -> Since this is how Unit type's toString() method works
//
// Supported overloads / operations and functions / methods:
//   - Constructor:            Quantity()
//   - Dimensionless factory:  Quantity::dimensionless()
//   - Addition:               operator+, operator+=
//   - Subtraction:            operator-, operator-=
//   - Unary negation:         operator-
//   - Multiplication:         operator*, operator*= (Quantity * Quantity, Quantity * scalar, scalar * Quantity)
//   - Division:               operator/, operator/= (Quantity / Quantity, Quantity / scalar, scalar / Quantity)
//   - Comparison:             operator<, operator<=, operator>, operator>=
//   - Minimum:                min()
//   - Maximum:                max()
//   - Absolute value:         abs()
//   - Exponentiation:         raisedTo(n)
//   - Printing:               print()
//   - Stream output:          operator<<
//   - std::format support:    std::format
//
// Example usage:
//   Unit meter(1, 0, 0, 0, 0, 0, 0);
//   Unit second(0, 0, 1, 0, 0, 0, 0);
//
//   Quantity length(5.0, meter);                            // 5 L^1
//   Quantity time(2.0, second);                             // 2 T^1
//
//   Quantity speed = length / time;                         // 2.5 L^1 T^-1
//   Quantity doubled = speed * 2.0;                         // 5.0 L^1 T^-1
//   Quantity acceleration = speed / time;                   // 1.25 L^1 T^-2
//
//   Quantity totalLength = length + Quantity{3.0, meter};   // 8 L^1
//   Quantity smallLength = length - Quantity{3.0, meter};   // 2 L^1
//
//   bool double > speed;                                    // true
//
//   Quantity smallest = Quantity::min(speed, double)        // speed
//
//   Quantity absolute = speed.abs();                        // 2.5 L^1 T^-1
//
//   Quantity squaredLength = length.raisedTo(2);            // 25 L^2
//
//   Quantity dimensionless = Quantity::dimensionless(42.0); // 42 dimensionless
//
//   std::cout << speed;                                     // Output: "2.5 L^1 T^-1"
//   speed.print();                                          // Output: "2.5 L^1 T^-1"
struct [[nodiscard]] Quantity {
    double value;
    Unit unit; // Dimensions represented by Unit struct

    friend std::ostream& operator<<(std::ostream& outputStream, const Quantity& quantity);

    // Default constructor
    constexpr Quantity() noexcept : value(0.0), unit(Unit::dimensionless()) {}

    // Dimensionless Quantity factory
    //
    // Returns a Quantity with a value of 1.0 and a Unit with all exponents zero
    // Used to make it clearer when there is dimensionless (and if non-specified valueless) Quantity initialisation
    [[nodiscard]] static Quantity dimensionless(double value = 0.0) noexcept {
        return {value, Unit::dimensionless()};
    }

    // Constructor from value only (dimensionless unit)
    constexpr explicit Quantity(const double value) noexcept : value(value), unit(Unit::dimensionless()) {}

    // Constructor from value and unit
    constexpr Quantity(const double value, const Unit& unit) noexcept : value(value), unit(unit) {}

    // Construct from value and unit string
    constexpr Quantity(const double value, const std::string_view unitString)
        : Quantity([&]() constexpr -> Quantity {
            auto [factor, siUnit] = parseUnits(unitString);
            return { value * factor, siUnit };
        }()) {}

    // Returns the raw numeric value
    //
    // Note: For testing only; not intended for production use
    [[nodiscard]] constexpr double asDouble() const { return this->value; }

    // Returns the unit as a Unit
    //
    // Note: For testing only; not intended for production use
    [[nodiscard]] constexpr Unit asUnit() const { return this->unit; }

    // Addition operator
    //
    // Requires same Unit
    [[nodiscard]] Quantity operator+(const Quantity& other) const {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot add quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        return {this->value + other.value, this->unit};
    }

    // Addition assignment operator
    //
    // Requires same Unit
    Quantity& operator+=(const Quantity& other) {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot add quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        this->value += other.value;
        return *this;
    }

    // Subtraction operator
    //
    // Requires same Unit
    [[nodiscard]] Quantity operator-(const Quantity& other) const {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot subtract quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        return {this->value - other.value, this->unit};
    }

    // Subtraction assignment operator
    //
    // Requires same Unit
    Quantity& operator-=(const Quantity& other) {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot subtract quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        this->value -= other.value;
        return *this;
    }

    // Unary negation operator
    [[nodiscard]] Quantity operator-() const {
        return {this->value * -1.0, this->unit};
    }

    // Multiplication operator
    //
    // Quantity * double
    [[nodiscard]] constexpr Quantity operator*(const double scalar) const noexcept {
        return {this->value * scalar, this->unit};
    }

    // Multiplication assignment operator
    //
    // Quantity *= double
    constexpr Quantity& operator*=(const double scalar) noexcept {
        this->value *= scalar;
        return *this;
    }

    // Multiplication operator
    //
    // Quantity * Quantity
    [[nodiscard]] constexpr Quantity operator*(const Quantity& other) const noexcept {
        return {this->value * other.value, this->unit * other.unit};
    }

    // Multiplication assignment operator
    //
    // Quantity *= Quantity
    constexpr Quantity& operator*=(const Quantity& other) noexcept {
        this->value *= other.value;
        this->unit *= other.unit;
        return *this;
    }

    // Division operator
    //
    // Quantity / double
    [[nodiscard]] Quantity operator/(const double scalar) const noexcept {
        return {this->value / scalar, this->unit};
    }

    // Division assignment operator
    //
    // Quantity /= double
    Quantity& operator/=(const double scalar) noexcept {
        this->value /= scalar;
        return *this;
    }

    // Division operator
    //
    // Quantity / Quantity
    [[nodiscard]] Quantity operator/(const Quantity& other) const noexcept {
        return {this->value / other.value, this->unit / other.unit};
    }

    // Division assignment operator
    //
    // Quantity /= Quantity
    Quantity& operator/=(const Quantity& other) noexcept {
        this->value /= other.value;
        this->unit /= other.unit;
        return *this;
    }

    // Less than operator
    bool operator<(const Quantity& other) const {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot compare (<) quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        return this->value < other.value;
    }

    // Less than or equal to operator
    bool operator<=(const Quantity& other) const {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot compare (<=) quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        return this->value <= other.value;
    }

    // Greater than operator
    bool operator>(const Quantity& other) const {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot compare (>) quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        return this->value > other.value;
    }

    // Greater than or equal to operator
    bool operator>=(const Quantity& other) const {
        if (this->unit != other.unit) {
            std::ostringstream oss;
            oss << "Cannot compare (>=) quantities with different units: lhs=" << *this << ", rhs=" << other;
            throw std::invalid_argument(oss.str());
        }
        return this->value >= other.value;
    }

    // Minimum method
    [[nodiscard]] static constexpr const Quantity& min(const Quantity& a, const Quantity& b) {
        return a <= b ? a : b;
    }

    // Maximum method
    [[nodiscard]] static constexpr const Quantity& max(const Quantity& a, const Quantity& b) {
        return a >= b ? a : b;
    }

    // Absolute value method
    Quantity abs() const {
        return {std::fabs(this->value), this->unit};
    }

    // Exponentiation method
    //
    // Raises a Quantity to any real power
    [[nodiscard]] Quantity raisedTo(const double power) const {
        return {std::pow(value, power), unit.raisedTo(power)};
    }

    // Print method
    void print() const {
        std::cout << this->value << " " << unit.toString();
    }
};

// Stream operator
inline std::ostream& operator<<(std::ostream& outputStream, const Quantity& quantity) {
    outputStream << quantity.value << " " << quantity.unit.toString();
    return outputStream;
}

// Multiplication operator
//
// Scalar * Quantity
[[nodiscard]] constexpr inline Quantity operator*(const double scalar, const Quantity& quantity) noexcept {
    return {scalar * quantity.value, quantity.unit};
}

// Division operator
//
// Scalar / Quantity
[[nodiscard]] constexpr inline Quantity operator/(const double scalar, const Quantity& quantity) noexcept {
    return {scalar / quantity.value, quantity.unit.inverse()};
}

template <typename CharT>
struct std::formatter<Quantity, CharT> : formatter<double, CharT> {
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return formatter<double, CharT>::parse(ctx);
    }

    template <class FormatContext>
    auto format(const Quantity& quantity, FormatContext& ctx) const {
        auto out = formatter<double, CharT>::format(quantity.value, ctx);
        const auto unit = quantity.unit.toString();
        *out++ = CharT(' ');
        for (char ch : unit) {
            *out++ = static_cast<CharT>(ch);
        }
        return out;
    }
};

// Table of some common physical constants
[[nodiscard]] inline const std::unordered_map<std::string_view, Quantity>& quantityTable() {
    using PhysConst = Globals::Constant::Physics;
    using ProgramConst = Globals::Constant::Program;
    static const std::unordered_map<std::string_view, Quantity> table = {
        {"time step",          {ProgramConst::timeStep,          Unit::timeDimension()}}, // Time step for simulation in seconds
        {"massless tolerance", {ProgramConst::masslessTolerance, Unit::massDimension()}}, // Massless tolerance in kg

        {"c",    {PhysConst::c,    Unit(1, 0, -1, 0,  0,  0, 0)}}, // Speed of light in vacuum
        {"e",    {PhysConst::e,    Unit(0, 0, 1,  1,  0,  0, 0)}}, // Elementary electric charge
        {"h",    {PhysConst::h,    Unit(2, 1, -1, 0,  0,  0, 0)}}, // Planck constant
        {"hbar", {PhysConst::hbar, Unit(2, 1, -1, 0,  0,  0, 0)}}, // Reduced Planck constant
        {"ℏ",    {PhysConst::hbar, Unit(2, 1, -1, 0,  0,  0, 0)}}, // Reduced Planck constant
        {"mu0",  {PhysConst::mu0,  Unit(1, 1, -2, -2, 0,  0, 0)}}, // Vacuum permeability
        {"k_b",  {PhysConst::k_b,  Unit(2, 1, -2, 0,  -1, 0, 0)}}  // Boltzmann constant
    };
    return table;
}

#endif //PHYSICS_SIMULATION_PROGRAM_QUANTITY_H