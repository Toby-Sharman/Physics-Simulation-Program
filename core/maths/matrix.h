//
// Physics Simulation Program
// File: matrix.h
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Describes Matrix custom data type and a specialised transformation matrix
//
// Copyright (c) 2025,
// Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_MATRIX_H
#define PHYSICS_SIMULATION_PROGRAM_MATRIX_H

#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"

#include <array>
#include <cmath>
#include <limits>
#include <ostream>
#include <stdexcept>

// Matrix<Rows, Columns>
//
// Represents a mathematical matrix in the form of an array of arrays of quantities as described by Quantity
//
// This class supports very basic matrix operations
//
// Notes on initialisation:
//
// Notes on algorithms:
//
// Notes on output:
//
// Supported overloads / operations and functions / methods:
//   - Constructor:            Matrix()
//   - Subscript:              operator[]
//   - Multiplication:         operator* (matrix * vector, matrix * matrix)
//   - Identity:               Matrix<rows, columns>::identity()
//   - Inverse:                inverse()
//   - Stream output:          operator<<
//
// Example usage: TODO
//   Matrix<3,3> rotation = Matrix<3,3>::identity();
//   std::cout << rotation[0];
//   auto inverse = rotation.inverse()
template <std::size_t Rows, std::size_t Columns>
struct Matrix {
    std::array<std::array<Quantity, Columns>, Rows> data; // Internal storage

    // Default constructor
    //
    // Dimensionless Quantity types with value 0.0
    Matrix() : data{} {
        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < Columns; ++j) {
                this->data[i][j] = Quantity(0.0);
            }
        }
    }

    // Constructor from a single Quantity (fills entire with that Quantity)
    explicit Matrix(const Quantity& value) : data {} {
        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < Columns; ++j) {
                this->data[i][j] = value;
            }
        }
    }

    // Constructor from initializer list of initializer lists
    Matrix(const std::initializer_list<std::initializer_list<Quantity>>& init) : data{} {
        std::size_t i = 0;
        for (auto& row : init) {
            std::size_t j = 0;
            for (auto& value : row) {
                if (i < Rows && j < Columns) {
                    this->data[i][j] = value;
                }
                ++j;
            }
            ++i;
        }
    }

    // Indexing
    //
    // Return mutable rows
    std::array<Quantity, Columns>& operator[](std::size_t row) {
        return this->data[row];
    }

    // Indexing
    //
    // Return read-only rows
    const std::array<Quantity, Columns>& operator[](std::size_t row) const {
        return this->data[row];
    }

    // Multiplication operator
    //
    // Matrix * Vector
    template<std::size_t N>
    Vector<Rows> operator*(const Vector<N>& vector) const {
        static_assert( N == Columns, "Matrix column and vector size mismatch");
        Vector<Rows> result;
        for (std::size_t i = 0; i < Rows; ++i) {
            result[i] = this->data[i][0] * vector[0];
            for (std::size_t j = 1; j < Columns; ++j) {
                result[i] += this->data[i][j] * vector[j];
            }
        }
        return result;
    }

    // Multiplication operator
    //
    // Matrix * matrix
    template<std::size_t OtherColumns>
    Matrix<Rows, OtherColumns> operator*(const Matrix<Columns, OtherColumns>& other) const {
        Matrix<Rows, OtherColumns> result;
        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < OtherColumns; ++j) {
                for (std::size_t k = 0; k < Columns; ++k) {
                    result[i][j] += this->data[i][k] * other[k][j];
                }
            }
        }
        return result;
    }

    // Identity matrix creation method
    static Matrix identity() {
        static_assert(Rows == Columns, "Identity only valid for square matrices");
        Matrix identityMatrix;
        for (std::size_t i = 0; i < Rows; ++i) {
            identityMatrix[i][i] = Quantity(1.0); // dimensionless Quantity types with value 1.0
        }
        return identityMatrix;
    }

    // Matrix inversion method
    [[nodiscard]] Matrix inverse() const {
        static_assert(Rows == Columns, "Matrix must be square for inversion");
        constexpr std::size_t n = Rows;

        Matrix<n, n> matrixA;     // raw numeric values
        Matrix<n, n> unitsMatrix; // corresponding units

        for (size_t i = 0; i < Rows; ++i) {
            for (size_t j = 0; j < Columns; ++j) {
                matrixA[i][j] = Quantity(this->data[i][j].value);         // numeric values
                unitsMatrix[i][j] = Quantity(1.0, this->data[i][j].unit); // units
            }
        }

        auto matrixI = Matrix::identity();

        constexpr double eps = std::numeric_limits<double>::epsilon() * 100.0; // Account for double limited precision

        for (std::size_t pivot = 0; pivot < n; ++pivot) {
            // Partial pivot: find max element in column pivot
            std::size_t maxRow = pivot;
            double maxValue = std::abs(matrixA[pivot][pivot].value);
            for (std::size_t candidateRow = pivot + 1; candidateRow < n; ++candidateRow) {
                if (const double value = std::abs(matrixA[candidateRow][pivot].value); value > maxValue) {
                    maxValue = value;
                    maxRow = candidateRow;
                }
            }

            if (maxValue <= eps) {
                throw std::runtime_error("Matrix is singular or nearly singular");
            }

            // Swap rows if needed
            if (maxRow != pivot) {
                std::swap(matrixA.data[pivot], matrixA.data[maxRow]);
                std::swap(matrixI.data[pivot], matrixI.data[maxRow]);
            }

            // Normalize pivot row
            Quantity pivotValue = matrixA[pivot][pivot];
            for (std::size_t column = 0; column < n; ++column) {
                matrixA[pivot][column] /= pivotValue;
                matrixI[pivot][column] /= pivotValue;
            }

            // Eliminate pivot column from all other rows
            for (std::size_t targetRow = 0; targetRow < n; ++targetRow) {
                if (targetRow == pivot) {
                    continue;
                }
                Quantity factor = matrixA[targetRow][pivot];
                for (std::size_t column = 0; column < n; ++column) {
                    matrixA[targetRow][column] -= factor * matrixA[pivot][column];
                    matrixI[targetRow][column] -= factor * matrixI[pivot][column];
                }
            }
        }

        return matrixI * unitsMatrix;
    }
};

// Stream operator
template<std::size_t Rows, std::size_t Columns>
std::ostream& operator<<(std::ostream& os, const Matrix<Rows, Columns>& matrix) {
    for (std::size_t i = 0; i < Rows; ++i) {
        os << "[ ";
        for (std::size_t j = 0; j < Columns; ++j) {
            os << matrix[i][j] << " ";
        }
        os << "]\n";
    }
    return os;
}

// TransformationMatrix
//
// Represents a transformation matrix encapsulating rotation and translation components
// TODO: Add scale component
//
// This class supports very basic transformations operations
//
// Notes on initialisation:
//
// Notes on algorithms:
//
// Notes on output:
//
// Supported overloads / operations and functions / methods:
//   - Multiplication:         operator* (transformation matrix * vector, transformation matrix * transformation matrix)
//   - As matrix:              asMatrix()
//   - Inverse:                inverse()
//
// Example usage: TODO
struct TransformationMatrix {
    Matrix<3,3> rotation;
    Vector<3> translation;

    // Multiplication operator
    //
    // Multiply transformation matrix by 3D vector (homogeneous transform)
    Vector<3> operator*(const Vector<3>& vector) {
        Vector<3> result;
        for (int i = 0; i < 3; ++i) {
            result[i] = this->translation[i];
            for (int j = 0; j < 3; ++j) {
                result[i] += this->rotation[i][j] * vector[j];
            }
        }
        return result;
    }

    // Multiplication operator
    //
    // Combine transformation matrices into one matrix
    TransformationMatrix operator*(const TransformationMatrix& other) const {
        TransformationMatrix result;
        result.rotation = this->rotation * other.rotation;
        result.translation = this->rotation * other.translation + this->translation;
        return result;
    }

    // As matrix method
    //
    // Converts the Transformation matrix into the standard 4x4 matrix
    [[nodiscard]] Matrix<4, 4> asMatrix() const {
        Matrix<4, 4> matrix;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                matrix[i][j] = this->rotation[i][j];
            }
        }
        matrix[0][3] = this->translation[0];
        matrix[1][3] = this->translation[1];
        matrix[2][3] = this->translation[2];
        matrix[3][0] = matrix[3][1] = matrix[3][2] = Quantity(0.0);
        matrix[3][3] = Quantity(1.0);
        return matrix;
    }

    [[nodiscard]] TransformationMatrix inverse() const {
        auto matrixInverse = asMatrix().inverse();

        TransformationMatrix inverted;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                inverted.rotation[i][j] = matrixInverse[i][j];
            }
            inverted.translation[i] = matrixInverse[i][3];
            inverted.translation[i] = matrixInverse[i][3];
            inverted.translation[i] = matrixInverse[i][3];
        }

        return inverted;
    }
};

#endif // PHYSICS_SIMULATION_PROGRAM_MATRIX_H