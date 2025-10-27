//
// Physics Simulation Program
// File: object.cpp
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Implementation of object.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

// TODO: Add a cache for world and inverse world transforms to reduce computations

#include "objects/object.h"

#include "core/maths/matrix.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/units.h"

[[nodiscard]] TransformationMatrix Object::getWorldTransformation() const noexcept{
    if (this->m_parent) {
        return this->m_parent->getWorldTransformation() * this->m_transformation;
    }
    return getLocalTransformation();
}

void Object::setPosition(const Vector<3> &position) {
    for (auto positionComponent : position) {
        if (positionComponent.unit != Unit(1,0,0,0,0,0,0)) {
            throw std::invalid_argument("Position component must be of length dimensions");
        }
    }
    this->m_transformation.translation = position;
}

void Object::setRotation(const Matrix<3, 3> &rotation) {
    for (std::size_t i = 0; i < 3; i++) {
        for (std::size_t j = 0; j < 3; j++) {
            if (rotation[i][j].unit != Unit(0,0,0,0,0,0,0)) {
                throw std::invalid_argument("Rotation components must be dimensionless");
            }
        }
    }
    this->m_transformation.rotation = rotation;
}

void Object::setTemperature(const Quantity temperature) {
    if (temperature.unit != Unit(0,0,0,0,1,0,0)) {
        throw std::invalid_argument("Temperature must be of temperature dimensions");
    }
    this->m_temperature = temperature;
}

void Object::setNumberDensity(const Quantity numberDensity) {
    if (numberDensity.unit != Unit(-3,0,0,0,0,0,0)) {
        throw std::invalid_argument("Number Density must be of length^-3 dimensions");
    }
    this->m_numberDensity = numberDensity;
}

[[nodiscard]] Vector<3> Object::localToWorldPoint(const Vector<3>& localPoint) const noexcept {
    return getWorldTransformation() * localPoint;
}

[[nodiscard]] Vector<3> Object::localToWorldDirection(const Vector<3> &localDirection) const noexcept {
    return getWorldTransformation().rotation * localDirection;
}

[[nodiscard]] Vector<3> Object::worldToLocalPoint(const Vector<3>& worldPoint) const noexcept {
    return getWorldTransformation().inverse() * worldPoint;
}

[[nodiscard]] Vector<3> Object::worldToLocalDirection(const Vector<3> &worldDirection) const noexcept {
    return getWorldTransformation().rotation.transpose() * worldDirection;
}

[[nodiscard]] const Object* Object::findObjectContaining(const Vector<3>& worldPoint) const noexcept {
    if (contains(worldPoint)) {
        for (const auto& child : m_children) {
            if (const auto* found = child->findObjectContaining(worldPoint)) {
                return found;
            }
        }
        return this;
    }
    return nullptr;
}

void Object::printHierarchy(const std::size_t indent) const {
    print(indent);
    for (const auto& child : this->m_children) {
        child->printHierarchy(indent + 2);
    }
}
