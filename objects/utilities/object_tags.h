//
// Created by Tobias Sharman on 08/09/2025.
//

#ifndef SIMULATION_PROGRAM_OBJECT_TAGS_H
#define SIMULATION_PROGRAM_OBJECT_TAGS_H

#include "vector.h"
#include "matrix.h"

struct NameTag { std::string value; };
inline NameTag Name(const std::string& s) { return NameTag{s}; }

struct MaterialTag { std::string value; };
inline MaterialTag Material(const std::string& s) { return MaterialTag{s}; }

struct PositionTag { Vector<3> value; };
inline PositionTag Position(const Vector<3>& v) { return PositionTag{v}; }

struct RotationTag { Matrix<double,3,3> value; };
inline RotationTag Rotation(const Matrix<double,3,3>& m) { return RotationTag{m}; }

struct SizeTag { Vector<3> value; };
inline SizeTag Size(const Vector<3>& v) { return SizeTag{v}; }

template <typename T>
concept NameArg = std::same_as<std::decay_t<T>, NameTag>;

template <typename T>
concept MaterialArg = std::same_as<std::decay_t<T>, MaterialTag>;

template <typename T>
concept PositionArg = std::same_as<std::decay_t<T>, PositionTag>;

template <typename T>
concept RotationArg = std::same_as<std::decay_t<T>, RotationTag>;

template <typename T>
concept SizeArg = std::same_as<std::decay_t<T>, SizeTag>;

#endif //SIMULATION_PROGRAM_OBJECT_TAGS_H