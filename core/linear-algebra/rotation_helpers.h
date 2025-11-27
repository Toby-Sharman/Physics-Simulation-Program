//
// Created by Tobias Sharman on 03/09/2025.
//
//TODO: Improve

#ifndef PHYSICS_SIMULATION_PROGRAM_ROTATION_HELPERS_H
#define PHYSICS_SIMULATION_PROGRAM_ROTATION_HELPERS_H

#include "matrix.h"
#include "vector.h"

#include "constants/maths.h"

#include <cmath>
#include <vector>
#include <tuple>

// Pitch/Yaw/Roll rotation (3x3, degrees)
inline Matrix<3,3> rotationMatrix3x3(const double pitch, const double yaw, const double roll) {
    const double p = pitch * constants::math::deg2rad; // Convert pitch
    const double y = yaw   * constants::math::deg2rad; // Convert yaw
    const double r = roll  * constants::math::deg2rad; // Convert roll

    const double cp = std::cos(p); const double sp = std::sin(p);
    const double cy = std::cos(y); const double sy = std::sin(y);
    const double cr = std::cos(r); const double sr = std::sin(r);

    Matrix<3,3> m = Matrix<3,3>::Identity();

    m[0][0] = cy * cr;                 m[0][1] = -cy * sr;                m[0][2] = sy;
    m[1][0] = sp * sy * cr + cp * sr;  m[1][1] = -sp * sy * sr + cp * cr; m[1][2] = -sp * cy;
    m[2][0] = -cp * sy * cr + sp * sr; m[2][1] = cp * sy * sr + sp * cr;  m[2][2] = cp * cy;

    return m;
}

// Pitch/Yaw/Roll rotation (4x4, degrees)
inline auto rotationMatrix4x4(const double pitch, const double yaw, double roll) -> Matrix<4, 4> {
    Matrix<3,3> m3 = rotationMatrix3x3(pitch, yaw, roll);
    Matrix<4,4> m4 = Matrix<4,4>::Identity();
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m4[i][j] = m3[i][j];
    return m4;
}

// Axis-angle rotation (3x3, axis must be normalized, angle in degrees)
inline Matrix<3,3> rotationMatrixAxisAngle3x3(const Vector<3>& axis, const double angle_deg) {
    const double angle = angle_deg * constants::math::deg2rad; // Convert angle
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    const double t = 1.0f - c;

    const double x = axis[0].asDouble();
    const double y = axis[1].asDouble();
    const double z = axis[2].asDouble();

    Matrix<3,3> m = Matrix<3,3>::Identity();
    m[0][0] = t*x*x + c;     m[0][1] = t*x*y - s*z;   m[0][2] = t*x*z + s*y;
    m[1][0] = t*x*y + s*z;   m[1][1] = t*y*y + c;     m[1][2] = t*y*z - s*x;
    m[2][0] = t*x*z - s*y;   m[2][1] = t*y*z + s*x;   m[2][2] = t*z*z + c;

    return m;
}

// Axis-angle rotation (4x4, axis must be normalized, angle in degrees)
inline Matrix<4,4> rotationMatrixAxisAngle4x4(const Vector<3>& axis, const double angle_deg) {
    Matrix<3,3> m3 = rotationMatrixAxisAngle3x3(axis, angle_deg);
    Matrix<4,4> m4 = Matrix<4,4>::Identity();
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m4[i][j] = m3[i][j];
    return m4;
}

// Chain multiple Euler rotations into one matrix (3x3)
inline Matrix<3,3> chainRotations3x3(const std::vector<std::tuple<double,double,double>>& rotations) {
    Matrix<3,3> R = Matrix<3,3>::Identity();
    for (auto& [pitch, yaw, roll] : rotations) {
        R = rotationMatrix3x3(pitch, yaw, roll) * R;
    }
    return R;
}

#endif //PHYSICS_SIMULATION_PROGRAM_ROTATION_HELPERS_H
