//
// Physics Simulation program
// File: object_initialisation_tags.h
// Created by Tobias Sharman on 20/10/2025
//
// Description:
//   - Tags for object initialisation clarity
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_INITIALISATION_TAGS_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_INITIALISATION_TAGS_H

#include <concepts>
#include <cstddef> // For std::size_t ignore warning
#include <memory>
#include <string>
#include <type_traits>

#include "core/maths/matrix.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"
#include "core/maths/utilities/units.h"

// Parent tag
//
// Allows for either unique pointers for the root object or Object class pointers for other objects
struct ParentTag {
    class Object* parent;
};
[[nodiscard]] constexpr ParentTag parent(Object* pointer) noexcept {
    return {pointer};
}
template<typename T>
[[nodiscard]] constexpr ParentTag parent(const std::unique_ptr<T>& object) noexcept
requires std::derived_from<T, Object> {
    return {object.get()};
}
template<typename T>
concept ParentArgument = std::same_as<std::decay_t<T>, ParentTag>;

// Name tag
//
// Allows for strings
struct NameTag {
    std::string value;
};
[[nodiscard]] constexpr NameTag name(const std::string& name) {
    return {name};
}
template <typename T>
concept NameArgument = std::same_as<std::decay_t<T>, NameTag>;

// Position tag
//
// Allows for a Vector<3> for position in (x, y, z)
struct PositionTag {
    Vector<3> value;
};
[[nodiscard]] constexpr PositionTag position(const Vector<3>& position) {
    for (auto positionComponent : position) {
        if (positionComponent.unit != Unit(1,0,0,0,0,0,0)) {
            throw std::invalid_argument("Position component must be of length dimensions");
        }
    }
    return {position};
}
template <typename T>
concept PositionArgument = std::same_as<std::decay_t<T>, PositionTag>;

// Rotation tag
//
// Allows for a standard 3x3 rotation matrix
struct RotationTag {
    Matrix<3,3> value;
};
[[nodiscard]] constexpr RotationTag rotation(const Matrix<3,3>& rotation) {
    for (std::size_t i = 0; i < 3; i++) {
        for (std::size_t j = 0; j < 3; j++) {
            if (rotation[i][j].unit != Unit(0,0,0,0,0,0,0)) {
                throw std::invalid_argument("Rotation components must be dimensionless");
            }
        }
    }
    return {rotation};
}
template <typename T>
concept RotationArgument = std::same_as<std::decay_t<T>, RotationTag>;

// Size tag
//
// Allows for a size of whatever type is needed for the object type initialising, i.e. Vector<3> for a cuboid or
// Quantity for a sphere
template <typename T>
struct SizeTag {
    T value;
};
template <typename T>
[[nodiscard]] constexpr SizeTag<T> size(const T& dimensions) {
    for (auto dimension : dimensions) {
        if (dimension.unit != Unit(1,0,0,0,0,0,0)) {
            throw std::invalid_argument("Size components must be of length dimensions");
        }
    }
    return {dimensions};
}
template <typename T>
concept SizeArgument = requires(T type) { type.value; };

// Material tag
//
// Allows for strings
struct MaterialTag {
    std::string value;
};
[[nodiscard]] constexpr MaterialTag material(const std::string& material) {
    return {material};
}
template <typename T>
concept MaterialArgument = std::same_as<std::decay_t<T>, MaterialTag>;

// Temperature tag
//
// Allows for Quantity types
struct TemperatureTag {
    Quantity value;
};
[[nodiscard]] constexpr TemperatureTag temperature(const Quantity& temperature) {
    if (temperature.unit != Unit(0,0,0,0,1,0,0)) {
        throw std::invalid_argument("Temperature must be of temperature dimensions");
    }
    return {temperature};
}
template <typename T>
concept TemperatureArgument = std::same_as<std::decay_t<T>, TemperatureTag>;

// Number density tag
//
// Allows for Quantity types
struct NumberDensityTag {
    Quantity value;
};
[[nodiscard]] constexpr NumberDensityTag numberDensity(const Quantity& numberDensity) {
    if (numberDensity.unit != Unit(-3,0,0,0,0,0,0)) {
        throw std::invalid_argument("Number Density must be of length^-3 dimensions");
    }
    return {numberDensity};
}
template <typename T>
concept NumberDensityArgument = std::same_as<std::decay_t<T>, NumberDensityTag>;

// Relative Permeability tag
//
// Allows for doubles
struct RelativePermeabilityTag {
    double value;
};
[[nodiscard]] constexpr RelativePermeabilityTag relativePermeability(const double& relativePermeability) noexcept {
    return {relativePermeability};
}
template <typename T>
concept RelativePermeabilityArgument = std::same_as<std::decay_t<T>, RelativePermeabilityTag>;

#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_INITIALISATION_TAGS_H