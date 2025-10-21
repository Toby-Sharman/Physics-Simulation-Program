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

// Generates the transformation from a local space to world space by recursively applying transformation matrices of all
// the parents of the specified child
[[nodiscard]] TransformationMatrix Object::getWorldTransformation() const noexcept{
    if (this->m_parent) {
        return this->m_parent->getWorldTransformation() * this->m_transformation;
    }
    return getLocalTransformation();
}

[[nodiscard]] Vector<3> Object::localToWorld(const Vector<3>& localPoint) const noexcept {
    return getWorldTransformation() * localPoint;
}

[[nodiscard]] Vector<3> Object::worldToLocal(const Vector<3>& worldPoint) const noexcept {
    return getWorldTransformation().inverse() * worldPoint;
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

void Object::printHierarchy(const int indent) const {
    print(indent);
    for (const auto& child : this->m_children) {
        child->printHierarchy(indent + 2);
    }
}
