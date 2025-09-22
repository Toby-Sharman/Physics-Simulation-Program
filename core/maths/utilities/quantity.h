//
// Created by Tobias Sharman on 16/09/2025.
//

#ifndef QUANTITY_H
#define QUANTITY_H

#include "unit_utilities.h"

#include <cmath>
#include <format>
#include <string>
#include <iostream>

// Quantity struct
struct Quantity {
    using Self = Quantity;

    double value;
    std::string unit;

    [[nodiscard]] double asDouble() const { return value; }
    [[nodiscard]] std::string asUnit() const { return unit; }

    // TODO: Addition. subtraction, multiplication, division, ?powers?, ?unary operations (e.g. trig)?, ?etc.?
    // Logic framework:
    // parse units ✅
    // If units are of valid type for operation (correct dimensions) (+, -, others?) ✅
    //      multiply value by scale -> new function for first three parts ✅
    //      perform operation (+, -, *, etc.) on value
    //      -> for non consistent units check for combo units (N, J, etc.) -> new function
    //      check size >=1000 or <0 and if so shift to prefix ✅
    //          -> take care for power units (m^3, etc.)

    [[nodiscard]] Quantity convertTo(const std::string& newUnit) const {
        const auto [scaleA, dimensionA] = parseUnits(unit);
        const auto [scaleB, dimensionB] = parseUnits(newUnit);

        if (dimensionA != dimensionB) {
            throw std::runtime_error(std::format("Cannot convert {} to {}",unit, newUnit));
        }

        return Quantity(value * (scaleA / scaleB), newUnit);
    }

    static Quantity withBestPrefix(Quantity quantity) { // TODO: Take care for units with powers
        auto [baseUnit, prefixScale] = extractPrefix(quantity.unit);
        quantity.value *= prefixScale;
        double absVal = std::fabs(quantity.value);

        if (absVal == 0.0) return quantity;

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

        // Map order → 10^order
        static const std::map<int, double> ORDER_TO_SCALE = {
            {-30, 1e-30}, {-27, 1e-27}, {-24, 1e-24}, {-21, 1e-21}, {-18, 1e-18},
            {-15, 1e-15}, {-12, 1e-12}, {-9, 1e-9},   {-6, 1e-6},   {-3, 1e-3},
            {3, 1e3},     {6, 1e6},     {9, 1e9},     {12, 1e12},   {15, 1e15},
            {18, 1e18},   {21, 1e21},   {24, 1e24},   {27, 1e27},   {30, 1e30}
        };

        if (const auto itScale = ORDER_TO_SCALE.find(order); itScale != ORDER_TO_SCALE.end()) {
            const double targetScale = itScale->second;

            // Search existing PREFIXES() for matching scale
            for (const auto&[symbol, scale] : PREFIXES()) {
                if (std::abs(scale - targetScale) < 1e-12) { // FIXME: Check if I need this
                    const double scaledValue = quantity.value / scale;
                    const std::string newUnit = std::format("{}{}", symbol, baseUnit);
                    return Quantity{scaledValue, newUnit};
                }
            }
        }

        // fallback
        return Quantity(quantity.value, baseUnit);
    }

    Self operator+(const Self& other) const {
        return Quantity(value + other.convertTo(unit).value, unit);
    }

    Self operator-(const Self& other) const {
        return Quantity(value - other.convertTo(unit).value, unit);
    }
};

// Non-member operator<< TODO: Format library support
inline std::ostream& operator<<(std::ostream& os, const Quantity& q) {
    os << q.value << " " << q.unit;
    return os;
}

#endif //QUANTITY_H