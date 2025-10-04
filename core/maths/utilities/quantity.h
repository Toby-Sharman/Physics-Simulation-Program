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

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

#include "core/maths/utilities/units.h"
#include "core/maths/utilities/unit_utilities.h"

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
//   - Printing:               print()
//   - Addition:               operator+, operator+=
//   - Subtraction:            operator-, operator-=
//   - Multiplication:         operator*, operator*= (Quantity * Quantity, Quantity * scalar, scalar * Quantity)
//   - Division:               operator/, operator/= (Quantity / Quantity, Quantity / scalar, scalar / Quantity)
//   - Stream output:          operator<<
//   - Dimensionless factory:  Quantity::dimensionless()
//
// Example usage:
//   Unit meter{{1,0,0,0,0,0,0}};
//   Unit second{{0,0,1,0,0,0,0}};
//
//   Quantity length{5.0, meter};                            // 5 L^1
//   Quantity time{2.0, second};                             // 2 T^1
//
//   Quantity speed = length / time;                         // 2.5 L^1 T^-1
//   Quantity doubled = speed * 2.0;                         // 5.0 L^1 T^-1
//   Quantity acceleration = speed / time;                   // 1.25 L^1 T^-2
//
//   Quantity totalLength = length + Quantity{3.0, meter};   // 8 L^1
//   Quantity smallLength = length - Quantity{3.0, meter};   // 2 L^1
//
//   Quantity dimensionless = Quantity::dimensionless(42.0); // 42 dimensionless
//
//   std::cout << speed;                                     // Output: "2.5 L^1 T^-1"
//   speed.print();                                          // Output: "2.5 L^1 T^-1"
struct Quantity {
    double value;
    Unit unit; // Dimensions represented by Unit struct

    // Default constructor
    Quantity() : value(0.0), unit(Unit::dimensionless()) {}

    // Constructor from value only (dimensionless unit)
    explicit Quantity(const double value) : value(value), unit(Unit::dimensionless()) {}

    // Constructor from value and unit
    Quantity(const double value, const Unit& unit) : value(value), unit(unit) {}

    // Construct from value and unit string
    Quantity(const double value, const std::string_view unit) : Quantity(fromValueAndUnitString(value, unit)) {}

    // Returns the raw numeric value
    //
    // Note: For testing only; not intended for production use
    [[nodiscard]] double asDouble() const { return this->value; }

    // Returns the unit as a Unit
    //
    // Note: For testing only; not intended for production use
    [[nodiscard]] Unit asUnit() const { return this->unit; }

    // Print method
    void print() const {
        std::cout << this->value << " " << unit.toString();
    }

    // Addition operator
    //
    // Requires same Unit
    [[nodiscard]] Quantity operator+(const Quantity& other) const {
        if (this->unit != other.unit)
            throw std::invalid_argument("Cannot add quantities with different units");
        return {this->value + other.value, this->unit};
    }

    // Addition assignment operator
    //
    // Requires same Unit
    Quantity& operator+=(const Quantity& other) {
        if (this->unit != other.unit)
            throw std::invalid_argument("Cannot add quantities with different units");
        this->value += other.value;
        return *this;
    }

    // Subtraction operator
    //
    // Requires same Unit
    [[nodiscard]] Quantity operator-(const Quantity& other) const {
        if (this->unit != other.unit)
            throw std::invalid_argument("Cannot subtract quantities with different units");
        return {this->value - other.value, this->unit};
    }

    // Subtraction assignment operator
    //
    // Requires same Unit
    Quantity& operator-=(const Quantity& other) {
        if (this->unit != other.unit)
            throw std::invalid_argument("Cannot subtract quantities with different units");
        this->value -= other.value;
        return *this;
    }

    // Multiplication operator
    //
    // Quantity * scalar
    [[nodiscard]] Quantity operator*(const double scalar) const noexcept {
        return {this->value * scalar, this->unit};
    }

    // Multiplication assignment operator
    //
    // Quantity *= scalar
    Quantity& operator*=(const double scalar) noexcept {
        this->value *= scalar;
        return *this;
    }

    // Multiplication operator
    //
    // Quantity * Quantity
    [[nodiscard]] Quantity operator*(const Quantity& other) const noexcept {
        return {this->value * other.value, this->unit * other.unit};
    }

    // Multiplication assignment operator
    //
    // Quantity *= Quantity
    Quantity& operator*=(const Quantity& other) noexcept {
        this->value *= other.value;
        this->unit *= other.unit;
        return *this;
    }

    // Division operator
    //
    // Quantity / scalar
    [[nodiscard]] Quantity operator/(const double scalar) const noexcept {
        return {this->value / scalar, this->unit};
    }

    // Division assignment operator
    //
    // Quantity /= scalar
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

    // Dimensionless Quantity factory
    //
    // Returns a Quantity with a value of 1.0 and a Unit with all exponents zero
    [[nodiscard]] static Quantity dimensionless(double value = 1.0) noexcept {
        return {value, Unit::dimensionless()};
    }

    private:
        // fromValueAndUnitString
        //
        // Helper for construction of a Quantity when using a string representation of units rather than a direct Unit
        // type
        //
        // Notes on inputs:
        //   - value can be any double
        //   - units must abide by the rules described by parseUnits for functional and correct parsing
        //
        // Parameters:
        //   - value - a double
        //   - units - an immutable string of characters
        //
        // Returns:
        //   - A Quantity type with a value and base SI dimensional representation
        [[nodiscard]] static Quantity fromValueAndUnitString(const double value, const std::string_view units) {
            auto [factor, siUnit] = parseUnits(units);
            return {value * factor, siUnit};
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
[[nodiscard]] inline Quantity operator*(const double scalar, const Quantity& quantity) noexcept {
    return {scalar * quantity.value, quantity.unit};
}

// Division operator
//
// Scalar / Quantity
[[nodiscard]] inline Quantity operator/(const double scalar, const Quantity& quantity) noexcept {
    return {scalar / quantity.value, quantity.unit.inverse()};
}

// Table of some common physical constants
[[nodiscard]] inline const std::unordered_map<std::string_view, UnitInfo>& quantityTable() {
    using PhysConst = Globals::Constant::Physics;
    static const std::unordered_map<std::string_view, UnitInfo> table = {
        {"c",    {PhysConst::c,    Unit(1, 0, -1, 0,  0, 0, 0)}}, // speed of light in vacuum
        {"h",    {PhysConst::h,    Unit(2, 1, -2, 0,  0, 0, 0)}}, // planck constant
        {"hbar", {PhysConst::hbar, Unit(2, 1, -2, 0,  0, 0, 0)}}, // reduced planck constant
        {"mu0",  {PhysConst::mu0,  Unit(1, 1, -2, -2, 0, 0, 0)}}  // vacuum permeability
    };
    return table;
}

#endif //PHYSICS_SIMULATION_PROGRAM_QUANTITY_H