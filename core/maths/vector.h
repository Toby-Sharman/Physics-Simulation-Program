//
// Created by Tobias Sharman on 01/09/2025.
//
/*
 * Data must be numbers
 * Valid units are defined in Units.h
 * More operations can be added - see some wiki ig
 */

// TODO: Add in unit handling for vector operations

#ifndef VECTOR_H
#define VECTOR_H

#include "units.h"
#include "maths_tags.h"

#include <iostream>
#include <array>
#include <string>
#include <algorithm> // For empty array checker
#include <cmath>
#include <stdexcept> // For index failures (out of range)
#include <format>

template <typename Container>
bool is_empty(const Container& c) {
    using ValueType = Container::value_type;
    return std::all_of(c.begin(), c.end(), [](const ValueType& x) { return x == ValueType{}; });
}// To check if arrays are emptied as .empty() does not work. Checks if different from default-initialised

// Generic Vector of size N
template <size_t N, typename T=double>
struct Vector {
    using Self = Vector<N>; // FIXME?
    std::array<T,N> data{};                          // Contents of the vector, e.g. (1,2,3)
    std::optional<std::array<std::string,N>> labels; // Labels for each index, e.g. (x,y,z) or (p_x,p_y,p_z)
    std::optional<std::array<std::string, N>> units; // Units for each index, e.g. (m,m,m) or (eV,kms^-1,kms^-1,kms^-1)

    // Empty vector
    Vector() = default;

    // Construct with data/labels/units
    explicit Vector(Data<T> d, std::optional<Labels> l = std::nullopt, std::optional<Units> u = std::nullopt) {
        if (d.values.size()!=N) throw std::invalid_argument(std::format(
            "Provided number of data points {} != Vector size {}", d.values.size(), N)
            );
        for (size_t i=0;i<N;i++) data[i]=d.values[i];

        if (l) {
            if (l->values.size()!=N) throw std::invalid_argument(std::format(
            "Provided number of labels {} != Vector size {}", l->values.size(), N)
            );
            labels = std::array<std::string,N>{};
            for (size_t i=0;i<N;i++) (*labels)[i]=l->values[i];
        }

        if (u) {
            if (u->values.size()!=N) throw std::invalid_argument(std::format(
            "Provided number of units {} != Vector size {}", u->values.size(), N)
            );
            units = std::array<std::string,N>{};
            for (size_t i=0;i<N;i++) (*units)[i]=u->values[i];
        }
    }

    Vector(const Vector& other) = default;       // copy constructor
    Vector(Vector&& other) = default;            // move constructor
    Vector& operator=(const Vector& other) = default;
    Vector& operator=(Vector&& other) = default;

    // Indexing by number
    double& operator[](size_t i) {
        if (i >= N) throw std::out_of_range("Vector index out of range");
        return data[i];
    } // Read & write
    const double& operator[](size_t i) const {
        if (i >= N) throw std::out_of_range("Vector index out of range");
        return data[i];
    } // Read only

    // Indexing by label
    double& operator[](const std::string& label) {
        for (size_t i = 0; i < N; i++) {
            if (labels[i] == label) return data[i];
        }
        throw std::out_of_range("Label not found");
    } // Read & write
    const double& operator[](const std::string& label) const {
        for (size_t i = 0; i < N; i++) {
            if (labels[i] == label) return data[i];
        }
        throw std::out_of_range("Label not found");
    } // Read only

    // Arithmetic operators - overload regular operators
    Self operator+(const Self& other) const {
        Self res;
        for (size_t i = 0; i < N; i++) res[i] = data[i] + other[i];
        return res;
    }
    Self operator-(const Self& other) const {
        Self res;
        for (size_t i = 0; i < N; i++) res[i] = data[i] - other[i];
        return res;
    }
    Self operator*(double scalar) const {
        Self res;
        for (size_t i = 0; i < N; i++) res[i] = data[i] * scalar;
        return res;
    }
    Self operator/(double scalar) const {
        Self res;
        for (size_t i = 0; i < N; i++) res[i] = data[i] / scalar;
        return res;
    }

    // Compound assignment operators
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

    // Dot products
    [[nodiscard]] double dot(const Self& other) const {
        double sum = 0;
        for (size_t i = 0; i < N; i++) sum += data[i] * other[i];
        return sum;
    }
    [[nodiscard]] double dot_metric(const Self& other, const std::array<int, N>& metric) const {
        double sum = 0;
        for (size_t i = 0; i < N; i++) sum += metric[i] * data[i] * other[i];
        return sum;
    } // For non-Euclidean spaces that may need a metric, e.g. Minkowski space

    // Length / Norm
    [[nodiscard]] double length_squared() const { return dot(*this); }
    [[nodiscard]] double length() const { return std::sqrt(length_squared()); }
    [[nodiscard]] Self normalized() const {
        const double len = length();
        if (len == 0) return Self(0);
        return *this * (1.0 / len);
    }
    [[nodiscard]] double length_squared_metric(const std::array<int, N>& metric) const {
        return dot_metric(*this, metric);
    } // For non-Euclidean spaces that may need a metric, e.g. Minkowski space

    // Print vector
    void print() const {
        if (!labels.has_value()) {
            std::cout << "(labels not set)\n";
            std::cout << "(";
            for (size_t i = 0; i < N; i++) {
                std::cout << data[i];
                if (i < N - 1) std::cout << ", ";
            }
            std::cout << ")\n";
            return;
        }
        const auto& lab = *labels; // dereference once
        std::cout << "(";
        for (size_t i = 0; i < N; i++) {
            std::cout << lab[i] << "=" << data[i];
            if (i < N - 1) std::cout << ", ";
        }
        std::cout << ")\n";
    }
};

// Non-member operator<<
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

// Non-member multiplication because multiplication is not commutative - this forces it to commute by reusing old function
// At least I think this works untested
template <size_t N>
Vector<N> operator*(double scalar, const Vector<N>& v) {
    return v * scalar;  // reuse the member operator
}

// Cross product specialization for 3D
inline Vector<3> cross(const Vector<3>& a, const Vector<3>& b) {
    return Vector<3>({
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    });
}

// Minkowski dot shortcut for Vector<4> (+,-,-,-)
inline double dot_minkowski(const Vector<4>& a, const Vector<4>& b) {
    static constexpr std::array<int, 4> metric = {+1, -1, -1, -1};
    return a.dot_metric(b, metric);
}

inline double length_squared_minkowski(const Vector<4>& a) {
    static constexpr std::array<int, 4> metric = {+1, -1, -1, -1};
    return a.length_squared_metric(metric);
}

#endif //VECTOR_H