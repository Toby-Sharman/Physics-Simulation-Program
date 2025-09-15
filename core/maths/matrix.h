//
// Created by Tobias Sharman on 03/09/2025.
//

// TODO: For the typename add options for multiple data types. i.e. float, double, int, etc. to keep it general for future projects
// TODO: Add concepts

#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

#include <stdexcept>
#include <cmath> // Ignore error, used for std::abs I think
#include <limits>
#include <type_traits>

template <typename T, size_t Rows, size_t Cols>
struct Matrix {
    std::array<std::array<T, Cols>, Rows> data{}; // Internal storage

    // Indexing
    std::array<T, Cols>& operator[](int row) { return data[row]; }
    const std::array<T, Cols>& operator[](int row) const { return data[row]; }

    [[nodiscard]] static size_t rows() { return Rows; }
    [[nodiscard]] static size_t cols() { return Cols; }

    // Identity Matrix
    static Matrix<T, Rows, Cols> Identity() {
        static_assert(Rows == Cols, "Identity only valid for square matrices");
        Matrix<T, Rows, Cols> I{};

        for (size_t i = 0; i < Rows; ++i) {
            I[i][i] = T(1); // diagonal elements set to 1
        }
        return I;
    }

    // Matrix * Vector multiplication
    template<int N>
    Vector<N> operator*(const Vector<N>& v) const {
        static_assert(N == Rows && N == Cols, "Matrix and vector dimension mismatch");
        Vector<N> result{{}, v.labels};
        for(int i=0;i<N;i++)
            for(int j=0;j<N;j++)
                result[i] += data[i][j] * v[j];
        return result;
    }

    // Matrix multiplication
    template<size_t OtherCols>
    Matrix<T,Rows,OtherCols> operator*(const Matrix<T,Cols,OtherCols>& other) const {
        Matrix<T,Rows,OtherCols> result{};
        for(int i=0;i<Rows;i++)
            for(int j=0;j<OtherCols;j++)
                for(int k=0;k<Cols;k++)
                    result[i][j] += data[i][k] * other[k][j];
        return result;
    }

    [[nodiscard]] Matrix<T, Rows, Cols> inverse() const {
        static_assert(Rows == Cols, "Matrix must be square for inversion");
        static_assert(std::is_floating_point_v<T>, "Matrix inverse requires floating-point type");

        constexpr size_t N = Rows;
        Matrix<T, N, N> A = *this;
        Matrix<T, N, N> I = Matrix<T, N, N>::Identity();

        const T eps = std::numeric_limits<T>::epsilon() * T(100); // tolerance

        for (size_t i = 0; i < N; ++i) {
            // Partial pivot: find max element in column i
            size_t pivotRow = i;
            T maxVal = std::abs(A[i][i]);
            for (size_t r = i + 1; r < N; ++r) {
                T val = std::abs(A[r][i]);
                if (val > maxVal) {
                    maxVal = val;
                    pivotRow = r;
                }
            }

            if (maxVal <= eps)
                throw std::runtime_error("Matrix is singular or nearly singular");

            // Swap rows in matrices A and I if needed
            if (pivotRow != i) {
                std::swap(A.data_[i], A.data_[pivotRow]);
                std::swap(I.data_[i], I.data_[pivotRow]);
            }

            // Normalize pivot row
            T pivot = A[i][i];
            for (size_t j = 0; j < N; ++j) {
                A[i][j] /= pivot;
                I[i][j] /= pivot;
            }

            // Eliminate column i from all other rows
            for (size_t k = 0; k < N; ++k) {
                if (k == i) continue;
                T factor = A[k][i];
                for (size_t j = 0; j < N; ++j) {
                    A[k][j] -= factor * A[i][j];
                    I[k][j] -= factor * I[i][j];
                }
            }
        }
        return I;
    }

private:
    Vector<Cols> data_[Rows];
};

template<typename T, size_t R, size_t C>
std::ostream& operator<<(std::ostream& os, const Matrix<T,R,C>& m) {
    for (size_t i = 0; i < R; ++i) {
        os << "[ ";
        for (size_t j = 0; j < C; ++j) {
            os << m[i][j] << " ";
        }
        os << "]\n";
    }
    return os;
}

// Specialization: Multiply 4x4 matrix by 3D vector (homogeneous transform
inline Vector<3> operator*(const Matrix<double,4,4>& m, const Vector<3>& v) {
    auto result = Vector<3>{};
    for (int i = 0; i < 3; i++) {
        result[i] = m[i][0] * v[0] +
                    m[i][1] * v[1] +
                    m[i][2] * v[2] +
                    m[i][3]; // translation
    }
    return result;
}


// Non-member operator for Matrix * Matrix
// TODO: Do I need this? Commented out until tested
// template<typename T, size_t Rows, size_t Cols, size_t OtherCols>
// Matrix<T,Rows,OtherCols> operator*(const Matrix<T,Rows,Cols>& a,
//                                           const Matrix<T,Cols,OtherCols>& b) {
//     Matrix<T, Rows, OtherCols> result{};
//     for (int i = 0; i < Rows; i++) {
//         for (int j = 0; j < OtherCols; j++) {
//             T sum = T(0);
//             for (int k = 0; k < Cols; k++) {
//                 sum += a[i][k] * b[k][j];
//             }
//             result[i][j] = sum;
//         }
//     }
//     return result;
// }

#endif //MATRIX_H