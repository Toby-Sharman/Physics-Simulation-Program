//
// Physics Simulation Program
// File: vector.h
// Created by Tobias Sharman on 01/09/2025
//
// Description:
//   - Describes Vector custom data type
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_VECTOR_H
#define PHYSICS_SIMULATION_PROGRAM_VECTOR_H

#include <array>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "core/maths/utilities/quantity.h"

// Vector<N>
//
// Represents a mathematical vector in the form of an array of quantities as described by Quantity
//
// This class supports common operator overloads (in particular arithmetic and stream operators) and dot product, cross
// product, length, squared length, unit vector, and printing methods
//
// Notes on initialisation:
//   - To make construction more general, i.e. typename T in the template for any T, would come at the cost of easy
//     Quantity type Vector construction via initializer list and optional shared unit since deduction guides cannot get
//     the size of an initializer list at compile time
//   - Can be constructed as a series of Quantity types
//   - Can be constructed as an array or initializer list of values with an optional shared unit
//   - Construction requires the same amount of values as the size of the Vector
//   - Neglected construction from a Unit type for the Unit to be passed to a Quantity constructor as this would be a
//     place of high likelihood of mistaken user input
//
// Notes on algorithms:
//   - Vector operations require the same dimensional vectors
//   - Addition and subtraction require identical units for same Vector index (dimensions must match exactly)
//   - Multiplication and division automatically combine units via Unit operators
//   - Cannot dot product an empty Vector
//   - Dot product on a 1d Vector will just to a Quantity multiplication operation
//   - Cannot generate a unit vector for a 0 length vector
//
// Notes on output:
//   - The unit is printed using the base dimension symbols (L, M, T, I, Θ, N, J)
//         -> Since this is how Unit type's toString() method works
//
// Supported overloads / operations and functions / methods:
//   - Constructor:            Vector()
//   - Subscript:              operator[]
//   - Begin:                  begin()
//   - End:                    end()
//   - Addition:               operator+, operator+=
//   - Subtraction:            operator-, operator-=
//   - Multiplication:         operator*, operator*=
//   - Division:               operator/, operator/=
//   - Dot product:            dot(vector)
//   - Cross product:          cross(vector)
//   - Length squared:         lengthSquared()
//   - Length:                 length()
//   - Unit vector:            unitVector()
//   - Printing:               print()
//   - Stream output:          operator<<
//
// Example usage: (The units have been described qualitatively when in reality they would be an array of dimensions)
//   std::array<double, 3> vals = {1.0, 2.0, 3.0};
//   Vector<3> v1(vals, "m");                      // v1 contains 3 Quantity types: {1.0 ,2.0 ,3.0}, dimensions of "m"
//   Vector<3> v2 = Vector({1.0, 2.0, 3.0});       // v2 contains 3 Quantity types: {1.0 ,2.0 ,3.0}, dimensions of ""
//   Vector<3> v3 = Vector({2.0, 6.0, 8.0}, "kg"); // v3 contains 3 Quantity types: {1.0 ,2.0 ,3.0}, dimensions of "kg"
//   const Vector<3> v4(Quantity{1.0, ""}, Quantity{2.0, ""}, Quantity{3.0, ""}); // Read-only Vector
//
//   v1[0] = Quantity{4.0, ""};                    // v1 index 0 is a Quantity with value of 4.0 and dimensions of ""
//   Quantity start = v2.begin()                   // start is a Quantity with value 1.0 and no dimensions
//   Quantity end = v2.end()                       // end is a Quantity with value 3.0 and no dimensions

//
//   Vector<3> v5 = v2 + v4;                       // v4 = {2.0, 4.0, 6.0} no dimensions
//   Vector<3> v6 = v2 - v4;                       // v6 = {0.0, 0.0, 0.0} no dimensions
//
//   double a = 3.0;
//   Vector<3> v7 = v3 * a;                        // v7 = {6.0, 18.0, 24.0} all dimensions of "kg"
//
//   double b = 2.0;
//   Vector<3> v8 = v3 / a;                        // v8 = {1.0, 3.0, 4.0} all dimensions of "kg"
//
//   Quantity q1 = v2.dot(v3);                     // q1 = 14 "kg"
//   Quantity q2 = dot(v2, v3);                    // q2 = 14 "kg"
//
//   Vector<3> v9 = v2.cross(v3);                  // v9 = {-2, -2, 2} all dimensions of "kg"
//   Vector<3> v10 = cross(v2, v3);                // v10 = {-2, -2, 2} all dimensions of "kg"
//
//   Quantity q3 = v3.lengthSquared();             // q3 = 14 "kg^2"
//   Quantity q4 = v3.length();                    // q4 = sqrt(14) "kg"
//
//   Vector<3> v11 = v3.unitVector;                // v11 = v3/q4
//
//   v1.print;                                     // prints "(1.0, 2.0 L, 3.0 L)"
//   std::cout << v1;                              // returns a stream "(1.0, 2.0 L, 3.0 L)"
template <std::size_t N>
struct [[nodiscard]] Vector {
    std::array<Quantity, N> data;  // Each element = value + unit

    Vector() = default;

    // Constructor from initializer list of Quantities
    Vector(const std::initializer_list<Quantity> quantities) {
        if (quantities.size() != N) {
            throw std::invalid_argument("Number of Quantity arguments must match vector dimension");
        }

        std::size_t i = 0;
        for (const auto& quantity : quantities) {
            data[i++] = quantity;
        }
    }

    // Variadic constructor for N elements that can be converted to a Quantity
    template <typename... Args>
    requires (std::is_convertible_v<Args, Quantity> && ...)
    explicit constexpr Vector(Args&&... args) : data{std::forward<Args>(args)...} {
        static_assert(sizeof...(Args) == N, "Number of arguments must match Vector size");
    }

    // Constructor from array of values and optional shared unit
    constexpr explicit Vector(const std::array<double, N>& values, const std::string& unit = "") : data{} {
        for (std::size_t i = 0; i < N; i++) {
            this->data[i] = Quantity{values[i], unit};
        }
    }

    // Constructor from initializer list of values and optional shared unit
    Vector(const std::initializer_list<double>& values, const std::string& unit = "") : data{} {
        if (values.size() != N) {
            throw std::invalid_argument("initializer_list size must match vector size");
        }
        std::size_t i = 0;
        for (const double value : values) {
            this->data[i++] = Quantity{value, unit};
        }
    }

    // Subscript operator
    //
    // Mutable
    Quantity& operator[](std::size_t i) {
        if (i >= N) {
            throw std::out_of_range("Vector index out of range");
        }
        return this->data[i];
    }

    // Subscript operator
    //
    // Read-only
    const Quantity& operator[](std::size_t i) const {
        if (i >= N) {
            throw std::out_of_range("Vector index out of range");
        }
        return this->data[i];
    }

    // Begin method
    //
    // Mutable
    [[nodiscard]] constexpr auto begin() noexcept {
        return data.begin();
    }

    // Begin method
    //
    // Read-only
    [[nodiscard]] constexpr auto begin() const noexcept {
        return data.begin();
    }

    // End method
    //
    // Mutable
    [[nodiscard]] constexpr auto end() noexcept {
        return data.end();
    }

    // End method
    //
    // Read-only
    [[nodiscard]] constexpr auto end() const noexcept {
        return data.end();
    }

    // Addition operator
    [[nodiscard]] Vector operator+(const Vector& other) const {
        Vector result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = data[i] + other[i];
        }
        return result;
    }

    // Addition assignment operator
    Vector& operator+=(const Vector& other) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = data[i] + other[i];
        }
        return *this;
    }

    // Subtraction operator
    [[nodiscard]] Vector operator-(const Vector& other) const {
        Vector result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = data[i] - other[i];
        }
        return result;
    }

    // Subtraction assignment operator
    Vector& operator-=(const Vector& other) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = data[i] - other[i];
        }
        return *this;
    }

    // Multiplication operator
    //
    // Vector * double
    [[nodiscard]] constexpr Vector operator*(double scalar) const noexcept {
        Vector result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = data[i] * scalar;
        }
        return result;
    }

    // Multiplication assignment operator
    //
    // Vector *= double
    constexpr Vector& operator*=(double scalar) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = data[i] * scalar;
        }
        return *this;
    }

    // Multiplication operator
    //
    // Vector * Quantity
    [[nodiscard]] constexpr Vector operator*(Quantity quantity) const noexcept {
        Vector result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = data[i] * quantity;
        }
        return result;
    }

    // Multiplication assignment operator
    //
    // Vector *= Quantity
    constexpr Vector& operator*=(Quantity quantity) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = data[i] * quantity;
        }
        return *this;
    }

    // Division operator
    //
    // Vector / double
    [[nodiscard]] Vector operator/(double scalar) const noexcept {
        Vector result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = data[i] / scalar;
        }
        return result;
    }

    // Division assignment operator
    //
    // Vector /= double
    Vector& operator/=(double scalar) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = data[i] / scalar;
        }
        return *this;
    }

    // Division operator
    //
    // Vector / Quantity
    [[nodiscard]] Vector operator/(Quantity quantity) const noexcept {
        Vector result;
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = data[i] / quantity;
        }
        return result;
    }

    // Division assignment operator
    //
    // Vector /= Quantity
    Vector& operator/=(Quantity quantity) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = data[i] / quantity;
        }
        return *this;
    }

    // Dot product method
    [[nodiscard]] Quantity dot(const Vector& other) const {
        if (N == 0) {
            throw std::invalid_argument("Cannot take dot product of empty vector");
        }

        Quantity result = data[0] * other[0];

        if (N == 1) {
            return result;
        }

        for (std::size_t i = 1; i < N; ++i) {
            result += data[i] * other[i];
        }

        return result;
    }

    // Cross product method
    [[nodiscard]] Vector<3> cross(const Vector<3>& other) const {
        static_assert(N == 3, "Cross product is only defined for 3D vectors");

        return Vector<3>{
            data[1] * other.data[2] - data[2] * other.data[1],
            data[2] * other.data[0] - data[0] * other.data[2],
            data[0] * other.data[1] - data[1] * other.data[0]
        };
    }

    // TODO: Minkowski?

    // Length squared method
    [[nodiscard]] Quantity lengthSquared() const { return dot(*this); }

    // Length method
    [[nodiscard]] Quantity length() const { return lengthSquared().raisedTo(0.5); }

    // Unit vector method
    [[nodiscard]] Vector unitVector() const {
        const auto len = length();
        if (len.value == 0) {
            throw std::invalid_argument("Cannot create a unit vector for a 0 length vector");
        }
        return *this / len;
    }

    // Print method
    void print() const {
        std::cout << "(";
        for (std::size_t i = 0; i < N; i++) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << data[i];
        }
        std::cout << ")\n";
    }
};

// Stream operator
template <std::size_t N>std::ostream& operator<<(std::ostream& outputStream, const Vector<N>& vector) {
    outputStream << "(";
    for (std::size_t i = 0; i < N; ++i) {
        if (i > 0) {
            outputStream << ", ";
        }
        outputStream << vector[i];
    }
    outputStream << ")";
    return outputStream;
}

// Multiplication operator
//
// double * Vector
template <std::size_t N>
[[nodiscard]] constexpr Vector<N> operator*(double scalar, const Vector<N>& vector) noexcept {
    return vector * scalar;
}

// Multiplication operator
//
// Quantity * Vector
template <std::size_t N>
[[nodiscard]] constexpr Vector<N> operator*(Quantity quantity, const Vector<N>& vector) noexcept {
    return vector * quantity;
}

// Dot product method
template <std::size_t N>
[[nodiscard]] Quantity dot(const Vector<N>& vector1, const Vector<N>& vector2) {
    if (N == 0) {
        throw std::invalid_argument("Cannot take dot product of empty vector");
    }

    Quantity result = vector1[0] * vector2[0];

    if (N == 1) {
        return result;
    }

    for (std::size_t i = 1; i < N; ++i) {
        result += vector1[i] * vector2[i];
    }

    return result;
}

// Cross product method
template <std::size_t N>
[[nodiscard]] Vector<N> cross(const Vector<N>& vector1, const Vector<N>& vector2) {
    static_assert(N == 3, "Cross product is only defined for 3D vectors");

    return Vector<3>{
        vector1.data[1] * vector2.data[2] - vector1.data[2] * vector2.data[1],
        vector1.data[2] * vector2.data[0] - vector1.data[0] * vector2.data[2],
        vector1.data[0] * vector2.data[1] - vector1.data[1] * vector2.data[0]
    };
}

#endif //PHYSICS_SIMULATION_PROGRAM_VECTOR_H
