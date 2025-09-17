//
// Created by Tobias Sharman on 01/09/2025.
//

#ifndef VECTOR_H
#define VECTOR_H

#include "quantity.h"

#include <iostream>
#include <array>
#include <string>
#include <cmath>
#include <stdexcept>
#include <format>
#include <initializer_list>
#include <type_traits>

// Generic Vector of size N
template <size_t N>
struct Vector {
    using Self = Vector<N>;
    std::array<Quantity, N> data{};  // each element = value + unit
    Vector() = default;
    ~Vector() = default;

    // -------------------------
    // Mixed constructor
    // -------------------------
    template<typename... Args>
    requires(sizeof...(Args) == N)
    explicit Vector(Args&&... args) {
        size_t idx = 0;

        auto tuple_args = std::forward_as_tuple(std::forward<Args>(args)...);

        auto process = [this, &idx]<typename T0>(T0&& arg) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, Quantity>) {
                data[idx++] = arg;
            } else if constexpr (std::is_arithmetic_v<T>) {
                data[idx++] = Quantity{static_cast<double>(arg), ""};
            // } else if constexpr (std::is_constructible_v<Quantity, T>) {
            //     // Handles {double, string} aggregate -> Quantity
            //     data[idx++] = Quantity{arg};
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type in Vector constructor");
            }
        };

        std::apply([&](auto&&... ts){ (process(ts), ...); }, tuple_args);
    }

    // -------------------------
    // Construct from array of doubles + optional shared unit
    // -------------------------
    explicit Vector(const std::array<double, N>& values,
                    const std::string& unit = "") {
        for (size_t i = 0; i < N; i++) {
            data[i] = Quantity{values[i], unit};
        }
    }

    // -------------------------
    // Construct from initializer_list of doubles + optional unit
    // -------------------------
    Vector(const std::initializer_list<double>& values,
           const std::string& unit = "") {
        if (values.size() != N)
            throw std::invalid_argument("Initializer list size must match vector size");
        size_t i = 0;
        for (const double value : values) {
            data[i++] = Quantity{value, unit};
        }
    }

    // -------------------------
    // Copy and move constructors
    // -------------------------
    Vector(const Vector& other) = default;
    Vector(Vector&& other) = default;
    Vector& operator=(const Vector& other) = default;
    Vector& operator=(Vector&& other) = default;
    
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
    Self operator+(const Self& other) const { // TODO: Units of the same dimension type can add
        Self res;
        for (size_t i = 0; i < N; i++) {
            if (data[i].unit != other[i].unit)
                throw std::invalid_argument("Unit mismatch in vector addition");
            res[i] = {data[i].value + other[i].value, data[i].unit};
        }
        return res;
    }

    Self operator-(const Self& other) const { // TODO: Units of the same dimension type can subtract
        Self res;
        for (size_t i = 0; i < N; i++) {
            if (data[i].unit != other[i].unit)
                throw std::invalid_argument("Unit mismatch in vector subtraction");
            res[i] = {data[i].value - other[i].value, data[i].unit};
        }
        return res;
    }

    Self operator*(double scalar) const { // TODO: Update prefixes for units
        Self res;
        for (size_t i = 0; i < N; i++) {
            res[i] = {data[i].value * scalar, data[i].unit};
        }
        return res;
    }

    Self operator/(double scalar) const { // TODO: Update prefixes for units
        Self res;
        for (size_t i = 0; i < N; i++) {
            res[i] = {data[i].value / scalar, data[i].unit};
        }
        return res;
    }

    // -------------------------
    // Compound assignment operators
    // -------------------------
    Self& operator+=(const Self& other) {
        for (size_t i = 0; i < N; i++) data[i] += other[i];
        return *this;
    }
    Self& operator-=(const Self& other) {
        for (size_t i = 0; i < N; i++) data[i] -= other[i];
        return *this;
    }
    Self& operator*=(double scalar) {
        for (size_t i = 0; i < N; i++) data[i] *= scalar;
        return *this;
    }
    Self& operator/=(double scalar) {
        for (size_t i = 0; i < N; i++) data[i] /= scalar;
        return *this;
    }

    // -------------------------
    // Length / Norm
    // -------------------------
    [[nodiscard]] double lengthSquared() const { return dot(*this); }
    [[nodiscard]] double length() const { return std::sqrt(lengthSquared()); }
    [[nodiscard]] Self normalized() const {
        const double len = length();
        if (len == 0) return Self(0);
        return *this * (1.0 / len);
    }

    // TODO: Readd metric length squared? (Non-euclidean etc.)

    // -------------------------
    // Dot product
    // -------------------------
    [[nodiscard]] double dot(const Self& other) const { // TODO: Unit handling
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

#endif // VECTOR_H
