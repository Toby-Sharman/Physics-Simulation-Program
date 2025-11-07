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

#include "objects/object.h"

#include <format>

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
    std::size_t componentIndex = 0;
    for (const auto& positionComponent : position) {
        if (positionComponent.unit != Unit(1,0,0,0,0,0,0)) {
            throw std::invalid_argument(std::format(
                "Object '{}' position[{}] must have length dimensions but got {}",
                m_name,
                componentIndex,
                positionComponent
            ));
        }
        ++componentIndex;
    }
    this->m_transformation.translation = position;
}

void Object::setRotation(const Matrix<3,3> &rotation) {
    for (std::size_t i = 0; i < 3; i++) {
        for (std::size_t j = 0; j < 3; j++) {
            if (const auto& component = rotation[i][j]; component.unit != Unit(0,0,0,0,0,0,0)) {
                throw std::invalid_argument(std::format(
                    "Object '{}' rotation[{}][{}] must be dimensionless but got {}",
                    m_name,
                    i,
                    j,
                    component
                ));
            }
        }
    }
    this->m_transformation.rotation = rotation;
}

void Object::setTemperature(const Quantity temperature) {
    if (temperature.unit != Unit(0,0,0,0,1,0,0)) {
        throw std::invalid_argument(std::format(
            "Object '{}' temperature must have thermodynamic temperature dimensions but got {}",
            m_name,
            temperature
        ));
    }
    this->m_temperature = temperature;
}

void Object::setNumberDensity(const Quantity numberDensity) {
    if (numberDensity.unit != Unit(-3,0,0,0,0,0,0)) {
        throw std::invalid_argument(std::format(
            "Object '{}' numberDensity must have length^-3 dimensions but got {}",
            m_name,
            numberDensity
        ));
    }
    this->m_numberDensity = numberDensity;
}

[[nodiscard]] Vector<3> Object::localToWorldPoint(const Vector<3>& localPoint) const noexcept {
    return getWorldTransformation() * localPoint;
}

[[nodiscard]] Vector<3> Object::worldToLocalPoint(const Vector<3>& worldPoint) const noexcept {
    return getWorldTransformation().inverse() * worldPoint;
}

[[nodiscard]] Vector<3> Object::localToWorldDirection(const Vector<3> &localDirection) const noexcept {
    return getWorldTransformation().rotation * localDirection;
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

Vector<3> Object::worldIntersection(const Vector<3>& startWorldPoint, const Vector<3>& worldDisplacement) const {
    const auto localStart = worldToLocalPoint(startWorldPoint);
    const auto localDisplacement = worldToLocalDirection(worldDisplacement);
    const auto intersectionLocal = localIntersection(localStart, localDisplacement);
    return localToWorldPoint(intersectionLocal);
}

void Object::printHierarchy(const std::size_t indent) const {
    print(indent);
    for (const auto& child : this->m_children) {
        child->printHierarchy(indent + 2);
    }
}