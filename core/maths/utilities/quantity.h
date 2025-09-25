//
// Physics Simulation Program
// File: quantity.h
// Created by Tobias Sharman on 16/09/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_QUANTITY_H
#define PHYSICS_SIMULATION_PROGRAM_QUANTITY_H

#include <cmath>
#include <concepts>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "core/maths/utilities/units.h"
#include "core/maths/utilities/unit_utilities.h"

// Represents a physical quantity with a value and a unit
// Supports arithmetic operations, unit conversions
struct Quantity {
    double value;
    std::string unit; // Operations will fail if the unit cannot be constructed from the PREFIXES and UNIT_TABLE keys

    // Default constructors for Quantity
    Quantity() : value(0.0) {} // Constructs a Quantity with value = 0.0 and unit = ""
    explicit Quantity(const double value) : value(value) {} // Constructs a Quantity with value = value and unit = ""

    // General constructor for Quantity
    //
    // Constructs a Quantity from a value and unit string
    // Allows construction with () rather than {}
    //
    // Parameters:
    //   value - any type convertible to double
    //   unit  - any type convertible to std::string - see class's unit variable declaration for limitations
    //
    // Example:
    //   Quantity g(9.81, "m/s^2");
    template<std::convertible_to<double> T, std::convertible_to<std::string> U>
    Quantity(T v, U&& u) : value(static_cast<double>(v)), unit(std::forward<U>(u)) {}

    // Returns the raw numeric value
    //
    // Note: For testing only; not intended for production use
    [[nodiscard]] double asDouble() const { return value; }

    // Returns the unit string
    //
    // Note: For testing only; not intended for production use
    [[nodiscard]] std::string asUnit() const { return unit; }

    // Print method for Quantity
    //
    // Prints the Quantity in the form "<value> <unit>"
    //
    // Example:
    //   Quantity q(5.0, "m");
    //   q.print(); -> 5 m
    void print() const {
        std::cout << value << " " << unit;
    }

    // convertTo function for Quantity
    //
    // Converts the Quantity to a new unit if they have the same dimensions else throws an error
    //
    // Parameters:
    //   newUnit - the target unit to convert into (e.g. m, s, etc.)
    //
    // Returns:
    //   An updated Quantity with new unit and reflected value changes
    //
    // Throws:
    //   invalid_argument if units have different dimensions
    //
    // Example:
    //   Quantity length{1000.0, "mm"};
    //   Quantity meters = length.convertTo("m");  -> meters is 1.0 m
    [[nodiscard]] Quantity convertTo(const std::string& newUnit) const { // TODO: Take care for units with powers (m^3) and use string view
        const auto [scaleA, dimensionA] = parseUnits(unit);
        const auto [scaleB, dimensionB] = parseUnits(newUnit);

        if (dimensionA != dimensionB) {
            throw std::invalid_argument(std::format("Cannot convert {} to {} due to mismatched dimensions",unit, newUnit));
        }

        return {value * (scaleA / scaleB), newUnit};
    }

    // withBestPrefix function for Quantity
    //
    // Adjusts the Quantity to use a suitable SI prefix and scales the value so that it falls in the range [1, 1000)
    //
    // Parameters:
    //   quantity - the Quantity to adjust
    //
    // Returns:
    //   A new Quantity with a scaled value and updated unit
    //
    // Throws:
    //   out_of_range if no defined prefix - if >10^30 or <10^-30
    //
    // Example:
    //   Quantity length{1500.0, "m"};
    //   Quantity scaled = Quantity::withBestPrefix(length);  -> scaled is 1.5 km
    static Quantity withBestPrefix(Quantity quantity) { // TODO: Take care for units with powers (m^3)
        if (quantity.value == 0.0) return quantity;
        // Would break the second while loop if zero was allowed and removes redundant operations

        auto [baseUnit, prefixScale] = extractPrefix(quantity.unit);
        quantity.value *= prefixScale;
        double absVal = std::fabs(quantity.value);

        int order = 0;
        while (absVal >= 1000.0) {
            absVal /= 1000.0;
            order += 3;
        }
        while (absVal < 1.0) {
            absVal *= 1000.0;
            order -= 3;
        }

        if (order == 0) return quantity;

        // Map doesn't link to PREFIXES map so if that is changed for some reason changes need to be reflected here
        // PREFIXES should never be changed so this should not be an issue
        static const std::unordered_map<int, std::string_view> orderToPrefix = {
            {-30, "q"}, {-27, "r"}, {-24, "y"}, {-21, "z"}, {-18, "a"},
            {-15, "f"}, {-12, "p"}, {-9, "n"},  {-6, "µ"},  {-3, "m"},
            {3, "k"},   {6, "M"},   {9, "G"},   {12, "T"},  {15, "P"},
            {18, "E"},  {21, "Z"},  {24, "Y"},  {27, "R"},  {30, "Q"}
        };

        // Find prefix in table else throw an error if it is too big or small
        std::string_view newPrefix;
        if (const auto it = orderToPrefix.find(order); it != orderToPrefix.end()) {
            newPrefix = it->second;
        } else {
            throw std::out_of_range(std::format("No SI prefix defined for order {}", order));
        }

        return Quantity{quantity.value, std::format("{}{}", newPrefix, baseUnit)};
    }

    // Addition operator for Quantity
    //
    // Adds one Quantity to another of the same dimensions
    //
    // Parameters:
    //   other - the Quantity to add
    //
    // Returns:
    //   A new Quantity representing the sum, expressed in this quantity’s unit
    //
    // Example:
    //   Quantity a{2.0, "m"};
    //   Quantity b{3.0, "km"};
    //   Quantity c = a + b;  -> c is 3002.0 m
    Quantity operator+(const Quantity& other) const {
        return {value + other.convertTo(unit).value, unit};
    }

    // Subtraction operator for Quantity
    //
    // Subtracts one Quantity from another of the same dimensions
    //
    // Parameters:
    //   other - the Quantity to subtract
    //
    // Returns:
    //   A new Quantity representing the difference, expressed in this quantity’s unit
    //
    // Example:
    //   Quantity a{2.0, "km"};
    //   Quantity b{3.0, "m"};
    //   Quantity c = a - b;  -> c is 1.997 km
    Quantity operator-(const Quantity& other) const {
        return {value - other.convertTo(unit).value, unit};
    }

    // Multiplication operator for Quantity
    //
    // Multiply a Quantity by a scalar in the order Quantity * scalar
    //
    // Parameters:
    //   scalar - amount to multiply by
    //
    // Returns:
    //   A new Quantity representing the product of the two
    //
    // Example:
    //   Quantity a{2.0, "m"};
    //   double b = 5.0
    //   Quantity c = a * b;  -> c is 10.0 m
    Quantity operator*(const double scalar) const {
        return {value * scalar, unit};
    }

    // Division operator for Quantity
    //
    // Divide a Quantity by a scalar in the order Quantity / scalar
    //
    // Parameters:
    //   scalar - amount to divide by
    //
    // Returns:
    //   A new Quantity representing the quotient of the two
    //
    // Example:
    //   Quantity a{2.0, "m"};
    //   double b = 5.0
    //   Quantity c = a / b;  -> c is 0.4 m
    Quantity operator/(const double scalar) const {
        return {value / scalar, unit};
    }

    // TODO: More operators overloads like * and / for two quantities
    // TODO: For functions that will create a new unit have some function to merge the units well
    // TODO: Add compound assignment operators
};

// << operator for Quantity
//
// Outputs a Quantity to a stream in the format "<value> <unit>"
//
// Parameters:
//   os        - the output stream
//   quantity  - the Quantity to print
//
// Returns:
//   A reference to the output stream for chaining
//
// Example:
//   Quantity distance{10.0, "m"};
//   std::cout << distance;  -> prints "10 m"
inline std::ostream& operator<<(std::ostream& os, const Quantity& quantity) {
    os << quantity.value << " " << quantity.unit;
    return os;
}

// Multiplication operator for Quantity
//
// Multiply a Quantity by a scalar in the order scalar * Quantity
//
// Parameters:
//   scalar - amount to multiply by
//
// Returns:
//   A new Quantity representing the product of the two
//
// Example:
//   double a = 5.0
//   Quantity b{2.0, "m"};
//   Quantity c = a * b;  -> c is 10.0 m
inline Quantity operator*(const double scalar, const Quantity& quantity) {
    return {scalar * quantity.value, quantity.unit};
}

// inline Quantity operator/(const double scalar, const Quantity& quantity) { // FIXME: Invert unit
//     return {scalar / quantity.value, quantity.unit};
// }

#endif //PHYSICS_SIMULATION_PROGRAM_QUANTITY_H