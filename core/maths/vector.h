//
// Physics Simulation Program
// File: vector.h
// Created by Tobias Sharman on 01/09/2025
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_VECTOR_H
#define PHYSICS_SIMULATION_PROGRAM_VECTOR_H

#include <array>
#include <cmath>
#include <format>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "core/maths/utilities/quantity.h"

// Anything that is okay to be used as a maths Vector index and can be made into a valid Quantity
// Helps with compile time errors
template<typename T>
concept ConvertibleToQuantity =
    std::same_as<std::decay_t<T>, Quantity> ||
    std::is_arithmetic_v<std::decay_t<T>> ||
    std::convertible_to<T, Quantity>;

// Generic Vector of size N
// Supports common vector operations
template <size_t N>
struct Vector {
    std::array<Quantity, N> data;  // Each element = value + unit

    // Variadic template constructor for Vector<N>
    //
    // Allows constructing a Vector with exactly N arguments of mixed types:
    //   A Quantity (stored directly) or a brace-init style that directly goes to a Quantity
    //   An arithmetic type (wrapped into a Quantity with an empty unit)
    //   Otherwise → either triggers a compile-time error or throws a runtime error
    //     Could only get to runtime error with very weird usage
    //
    // Template Parameters:
    //   Args... - a pack of argument types, deduced per call
    //
    // Parameters:
    //   args... - a pack of argument values, forwarded to the constructor
    //
    // Returns:
    //   Custom Vector type - fixed size array of custom Quantity types
    //
    // Constraints:
    //   requires(sizeof...(Args) == N)
    //     Ensures the number of arguments matches the dimension of the Vector.
    //
    // Example:
    //   Vector<3> v(
    //       1,                    // arithmetic -> Quantity{1.0, ""}
    //       {9.81, "m/s^2"},      // brace-init -> Quantity{9.81, "m/s^2"}
    //       Quantity{3.14, "rad"} // already a Quantity
    //   );
    template<ConvertibleToQuantity... Args>
    requires(sizeof...(Args) == N)
    explicit(false) Vector(Args&&... args)
        : data{convert_to_quantity(std::forward<Args>(args))...} {}

    // Construct from array of doubles + optional shared unit
    //
    // Maps array values to the value of a Quantity
    // Sets all units to the unit described by the optional unit
    //
    // Parameters:
    //   values - std::array of doubles, one per Vector element
    //   unit   - optional unit string to assign to all elements (default: "")
    //
    // Returns:
    //   Custom Vector type - fixed size array of custom Quantity types
    //
    // Example:
    //   std::array<double, 3> vals = {1.0, 2.0, 3.0};
    //   Vector<3> v(vals, "m"); -> v contains 3 Quantity types: 1 m, 2 m, 3 m
    explicit Vector(const std::array<double, N>& values,
                    const std::string& unit = "") {
        for (size_t i = 0; i < N; i++) {
            data[i] = Quantity{values[i], unit};
        }
    }

    // Construct from initializer_list of doubles + optional shared unit
    // Maps initializer_list values to the value of a Quantity
    // Sets all units to the unit described by the optional unit
    //
    // Parameters:
    //   values - initializer_list of doubles; size must match N
    //   unit   - optional unit string to assign to all elements (default: "")
    //
    // Returns:
    //   Custom Vector type - fixed size array of custom Quantity types
    //
    // Throws:
    //   Error if passed an initializer_list of different size to desired vector
    //
    // Example:
    //   Vector<3> v = Vector<3>({1.0, 2.0, 3.0});               -> v contains 3 Quantity types: 1, 2, 3 (unit = "")
    //   Vector<3> vWithUnit = Vector<3>({1.0, 2.0, 3.0}, "kg"); -> v contains 3 Quantity types: 1 kg, 2 kg, 3 kg
    Vector(const std::initializer_list<double>& values,
           const std::string& unit = "") {
        if (values.size() != N)
            throw std::invalid_argument("initializer_list size must match vector size");
        size_t i = 0;
        for (const double value : values) {
            data[i++] = Quantity{value, unit};
        }
    }

    // -------------------------
    // Indexing
    // -------------------------
    Quantity& operator[](size_t i) {
        if (i >= N) throw std::out_of_range("Vector index out of range");
        return data[i];
    }
    const Quantity& operator[](size_t i) const {
        if (i >= N) throw std::out_of_range("Vector index out of range");
        return data[i];
    }

    // -------------------------
    // Print vector
    // -------------------------
    void print() const { // TODO: Rework with format
        std::cout << "(";
        for (size_t i = 0; i < N; i++) {
            std::cout << data[i].value;
            if (!data[i].unit.empty()) std::cout << " " << data[i].unit;
            if (i < N - 1) std::cout << ", ";
        }
        std::cout << ")\n";
    }

    // -------------------------
    // Arithmetic operators
    // -------------------------
    Vector<N> operator+(const Vector<N>& other) const {
        Vector<N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    Vector<N> operator-(const Vector<N>& other) const {
        Vector<N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    Vector<N> operator*(double scalar) const {
        Vector<N> res;
        for (size_t i = 0; i < N; i++) {
            res[i] = {data[i] * scalar, data[i].unit};
        }
        return res;
    }

    Vector<N> operator/(double scalar) const {
        Vector<N> res;
        for (size_t i = 0; i < N; i++) {
            res[i] = {data[i] / scalar, data[i].unit};
        }
        return res;
    }

    // -------------------------
    // Compound assignment operators
    // -------------------------
    Vector<N>& operator+=(const Vector<N>& other) {
        for (size_t i = 0; i < N; i++) data[i] += other[i];
        return *this;
    }
    Vector<N>& operator-=(const Vector<N>& other) {
        for (size_t i = 0; i < N; i++) data[i] -= other[i];
        return *this;
    }
    Vector<N>& operator*=(double scalar) {
        for (size_t i = 0; i < N; i++) data[i] *= scalar;
        return *this;
    }
    Vector<N>& operator/=(double scalar) {
        for (size_t i = 0; i < N; i++) data[i] /= scalar;
        return *this;
    }

    // -------------------------
    // Length / Norm
    // -------------------------
    [[nodiscard]] double lengthSquared() const { return dot(*this); }
    [[nodiscard]] double length() const { return std::sqrt(lengthSquared()); }
    [[nodiscard]] Vector<N> normalized() const {
        const double len = length();
        if (len == 0) return Vector<N>(0);
        return *this * (1.0 / len);
    }

    // TODO: Readd metric length squared? (Non-euclidean etc.)

    // -------------------------
    // Dot product
    // -------------------------
    [[nodiscard]] double dot(const Vector<N>& other) const { // TODO: Unit handling
        double sum = 0;
        for (size_t i = 0; i < N; i++) {
            if (data[i].unit != other[i].unit)
                throw std::invalid_argument("Unit mismatch in dot product");
            sum += data[i].value * other[i].value;
        }
        return sum;
    }

    // TODO: Readd metric dot? (Non-euclidean etc.)


    // TODO: Cross product
    // TODO: Minkowski
    private:
        // Constructs a Quantity from a variety of inputs
        // Separate to Quantity's own constructor to allow for empty units,
        // which would make no real sense for standard Quantity usage
        //
        // Parameters:
        //   arg - anything that can be converted into a Quantity or is arithmetic
        //
        // Returns:
        //   A custom Quantity data type as defined in quantity.h
        //
        // Throws:
        //   Error if passed an argument that cannot be made into a maths Vector
        //
        // Example:
        //   Quantity a = convertToQuantity(arg);
        static Quantity convertToQuantity(auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_arithmetic_v<T>) {
                // Number → Quantity with value = arg, unit = ""
                return Quantity{static_cast<double>(arg), ""};
            } else if constexpr (std::same_as<T, Quantity>) {
                // Quantity → Just forwards arg through directly as is already correct type
                return std::forward<T>(arg);
            } else if constexpr (std::convertible_to<T, Quantity>) {
                // Brace-init list or convertible type → make a Quantity
                return Quantity{std::forward<T>(arg)};
            } else {
                throw std::invalid_argument("Vector constructor argument cannot be converted to Quantity");
            }
        }
};

// -------------------------
// Non-member operator <<
// -------------------------
template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const Vector<N>& v) {
    os << "(";
    for (std::size_t i = 0; i < N; ++i) {
        os << v[i].value;
        if (!v[i].unit.empty()) os << " " << v[i].unit;
        if (i != N - 1) os << ", ";
    }
    os << ")";
    return os;
}

// -------------------------
// Non-member multiplication (scalar * vector)
// -------------------------
template <size_t N>
Vector<N> operator*(double scalar, const Vector<N>& v) {
    return v * scalar;
}

#endif //PHYSICS_SIMULATION_PROGRAM_VECTOR_H
