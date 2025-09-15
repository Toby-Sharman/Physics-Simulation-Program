//
// Created by Tobias Sharman on 08/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_TAGS_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_TAGS_H

#include "vector.h"
#include "matrix.h"

struct NameTag { std::string value; };
inline NameTag Name(const std::string& s) { return NameTag{s}; }
template <typename T> concept NameArg = std::same_as<std::decay_t<T>, NameTag>;

struct MaterialTag { std::string value; };
inline MaterialTag Material(const std::string& s) { return MaterialTag{s}; }
template <typename T> concept MaterialArg = std::same_as<std::decay_t<T>, MaterialTag>;

struct PositionTag { Vector<3> value; };
inline PositionTag Position(const Vector<3>& v) { return PositionTag{v}; }
template <typename T> concept PositionArg = std::same_as<std::decay_t<T>, PositionTag>;

struct RotationTag { Matrix<double,3,3> value; };
inline RotationTag Rotation(const Matrix<double,3,3>& m) { return RotationTag{m}; }
template <typename T> concept RotationArg = std::same_as<std::decay_t<T>, RotationTag>;

struct SizeTag { Vector<3> value; };
inline SizeTag Size(const Vector<3>& v) { return SizeTag{v}; }
template <typename T> concept SizeArg = std::same_as<std::decay_t<T>, SizeTag>;

#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_TAGS_H