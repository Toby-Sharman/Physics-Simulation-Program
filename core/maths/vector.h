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
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

#include "core/maths/utilities/quantity.h"
#include "core/maths/utilities/units.h"

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
template <size_t N>
struct Vector {
    std::array<Quantity, N> data{};  // Each element = value + unit

    // Array constructor for Vector<N>
    //
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
    explicit Vector(const std::array<double, N>& values, const std::string& unit = "") : data{} {
        for (size_t i = 0; i < N; i++) {
            this->data[i] = Quantity{values[i], unit};
        }
    }

    // Initializer list constructor for Vector<N>
    //
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
    //   invalid_argument if passed an initializer_list of different size to desired vector
    //
    // Example:
    //   Vector<3> v = Vector<3>({1.0, 2.0, 3.0}); -> v contains 3 Quantity types: 1, 2, 3 (unit = "")
    //   Vector<3> vWithUnit = Vector<3>({1.0, 2.0, 3.0}, "kg"); -> v contains 3 Quantity types: 1 kg, 2 kg, 3 kg
    Vector(const std::initializer_list<double>& values, const std::string& unit = "") : data{} {
        if (values.size() != N)
            throw std::invalid_argument("initializer_list size must match vector size");
        size_t i = 0;
        for (const double value : values) {
            this->data[i++] = Quantity{value, unit};
        }
    }

    // Subscript operator for Vector<N>
    //
    // Provides element access by index, with two overloads:
    //   Non-const version: returns a reference to allow modification of the element
    //   Const version: returns a const reference to allow read-only access
    //
    // Parameters:
    //   i - Index of the element to access
    //
    // Returns:
    //   Reference (or const reference) to the Quantity stored at position i
    //
    // Throws:
    //   out_of_range if i >= N, ensuring safe bounds-checked access
    //
    // Examples:
    //   Non-const usage (modify elements):
    //     Vector<3> v{1, 2, 3};
    //     v[0] = Quantity{4.0, ""}; -> sets the first element
    //     v[2] += Quantity{1.5, ""}; -> modifies the third element
    //
    //   Const usage (read-only access):
    //     const Vector<3> cv{Quantity{1.0, ""}, Quantity{2.0, ""}, Quantity{3.0, ""}};
    //     auto x = cv[1]; -> reads the second element, cannot modify
    Quantity& operator[](size_t i) {
        if (i >= N) throw std::out_of_range("Vector index out of range");
        return this->data[i];
    }
    const Quantity& operator[](size_t i) const {
        if (i >= N) throw std::out_of_range("Vector index out of range");
        return this->data[i];
    }

    // Print method for Vector<N>
    //
    // Outputs the contents of the vector to std::cout in a human-readable format.
    // Each element is displayed as its value followed by its unit (if any), separated by commas,
    // and enclosed in parentheses.
    //
    // This method does not modify the vector and is marked as const.
    //
    // Parameters:
    //   None
    //
    // Returns:
    //   void
    //
    // Examples:
    //   Vector<3> v{Quantity{1.0, "m"}, Quantity{2.5, "s"}, Quantity{3.0, ""}};
    //   v.print(); -> Output: (1 m, 2.5 s, 3)
    void print() const {
        std::cout << "(";
        for (size_t i = 0; i < N; i++) {
            this->data[i].print();            // delegate to Quantity's print
            if (i < N - 1) std::cout << ", ";
        }
        std::cout << ")\n";
    }

    // Addition operator for Vector<N>
    //
    // Adds one Vector to another of the same size using the Quantity types overloads
    //
    // Parameters:
    //   other - the Vector to add
    //
    // Returns:
    //   A new Vector representing the sum of the two
    //
    // Example:
    //   Vector<3> a({1.0, 2.0, 3.0});
    //   Vector<3> b({5.0, 7.0, 9.0});
    //   Vector<3> c = a + b;  -> c is {6.0, 9.0, 12.0} (neglecting units for example)
    Vector<N> operator+(const Vector<N>& other) const {
        Vector<N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = this->data[i] + other[i];
        }
        return result;
    }

    // Subtraction operator for Vector<N>
    //
    // Subtracts one Vector from another of the same size using the Quantity types overloads
    //
    // Parameters:
    //   other - the Vector to subtract
    //
    // Returns:
    //   A new Vector representing the difference of the two
    //
    // Example:
    //   Vector<3> a({1.0, 0.0, 3.0});
    //   Vector<3> b({5.0, 7.0, 9.0});
    //   Vector<3> c = a - b;  -> c is {-4.0, 2.0, -6.0} (neglecting units for example)
    Vector<N> operator-(const Vector<N>& other) const {
        Vector<N> result;
        for (size_t i = 0; i < N; i++) {
            result[i] = this->data[i] - other[i];
        }
        return result;
    }

    // Multiplication operator for Vector<N>
    //
    // Multiplies one Vector by a scalar in the order Quantity * scalar
    //
    // Parameters:
    //   scalar - amount to multiply by
    //
    // Returns:
    //   A new Vector representing the product of the two
    //
    // Example:
    //   Vector<3> a({1.0, 2.0, 3.0});
    //   double b = 3.0
    //   Vector<3> c = a * b;  -> c is {3.0, 6.0, 9.0} (neglecting units for example)
    Vector<N> operator*(double scalar) const {
        Vector<N> res;
        for (size_t i = 0; i < N; i++) {
            res[i] = {this->data[i] * scalar};
        }
        return res;
    }

    // Division operator for Vector<N>
    //
    // Divides one Vector by a scalar in the order Quantity / scalar
    //
    // Parameters:
    //   scalar - amount to divide by
    //
    // Returns:
    //   A new Vector representing the quotient of the two
    //
    // Example:
    //   Vector<3> a({1.0, 2.0, 3.0});
    //   double b = 2.0
    //   Vector<3> c = a / b;  -> c is {0.5, 1.0, 1.5} (neglecting units for example)
    Vector<N> operator/(double scalar) const {
        Vector<N> res;
        for (size_t i = 0; i < N; i++) {
            res[i] = {this->data[i] / scalar, this->data[i].unit};
        }
        return res;
    }

    // Compound assignment operators for Vector<N>
    //
    // Performs an in-place arithmetic operation (addition, subtraction, multiplication, or division) on this Vector
    // For addition and subtraction supports Vectors
    // For multiplication and division supports scalars
    //
    // Supported operators: +=, -=, *=, /=
    //
    // Parameters:
    //   value - the scalar or Vector to combine with this Vector
    //
    // Returns:
    //   A reference to this Vector after the operation
    //
    // Examples:
    //   Vector<3> a({1.0, 2.0, 3.0});
    //   Vector<3> b({4.0, 1.0, 12.0});
    //   double c = 2.0;
    //
    //   a += b;  // {5.0, 3.0, 15.0}
    //   a -= b;  // {-3.0, 1.0, -9.0}
    //
    //   a *= c;  // {2.0, 4.0, 6.0}
    //   a /= c;  // {0.5, 1.0, 1.5}
    Vector<N>& operator+=(const Vector<N>& other) {
        // Addition
        *this = *this + other;
        return *this;
    }
    Vector<N>& operator-=(const Vector<N>& other) {
        // Subtraction
        *this = *this - other;
        return *this;
    }
    Vector<N>& operator*=(double scalar) {
        // Multiplication by scalar
        *this = *this * scalar;
        return *this;
    }
    Vector<N>& operator/=(double scalar) {
        // Division by scalar
        *this = *this / scalar;
        return *this;
    }

    // -------------------------
    // Dot product
    // ------------------------- TODO: Good handling of resultUnit
    Quantity dot(const Vector<N>& other) const {
        double accumulatedValue = 0.0;
        Unit resultDimension;
        std::optional<std::string> resultUnit;

        for (std::size_t i = 0; i < N; ++i) {
            const auto [scaleA, dimensionA] = parseUnits(this->data[i].unit);
            const auto [scaleB, dimensionB] = parseUnits(other.data[i].unit);

            const double productValue = scaleA * this->data[i].value * scaleB * other.data[i].value;
            Unit combinedDimension = dimensionA + dimensionB;

            if (!resultUnit.has_value()) {
                resultDimension = combinedDimension;
                resultUnit = this->data[i].unit + "*" + other[i].unit;
            } else if (resultDimension != combinedDimension) {
                throw std::invalid_argument("Unit mismatch in dot product");
            }

            accumulatedValue += productValue;
        }

        return Quantity{accumulatedValue, *resultUnit};
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

    // TODO: Readd metric dot? (Non-euclidean etc.)


    // TODO: Cross product
    // TODO: Minkowski
};

// << operator for Vector<N>
//
// Outputs a Vector to a stream in the format "(<value> <unit>, ...)"
//
// Parameters:
//   os      - the output stream
//   vector  - the Vector<N> to print
//
// Returns:
//   A reference to the output stream for chaining
//
// Example:
//   Vector<3> a({1.0, 2.0, 3.0}, "m");
//   std::cout << a;  -> prints "(1.0 m, 2.0 m, 3.0 m)"
template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const Vector<N>& v) {
    os << "(";
    for (std::size_t i = 0; i < N; ++i) {
        os << v[i];
        if (i != N - 1) os << ", ";
    }
    os << ")";
    return os;
}

// Multiplication operator for Vector<N>
//
// Multiply a Vector<N> by a scalar in the order Vector<N> * scalar
//
// Parameters:
//   scalar - amount to multiply by
//
// Returns:
//   A new Vector<N> representing the product of the two
//
// Example:
//   double a = 3.0
//   Vector<3> b({1.0, 2.0, 3.0});
//   Vector<3> c = a * b;  -> c is {3.0, 6.0, 9.0} (neglecting units for example)
template <size_t N>
Vector<N> operator*(double scalar, const Vector<N>& v) {
    return v * scalar;
}

#endif //PHYSICS_SIMULATION_PROGRAM_VECTOR_H
