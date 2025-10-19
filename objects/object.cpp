//
// Created by Tobias Sharman on 03/09/2025.
//

// TODO: Add remove object functionality
// TODO: Add a cache for world and inverse world transforms to reduce computations

#include "matrix.h"
#include "vector.h"

#include "material_database.h"

#include "object.h"

#include <format>
#include <iostream>
#include <memory> // Do I need this?
#include <stdexcept>
#include <string>

// Getters
TransformationMatrix Object::getLocalTransform() const {
    return { this->m_transformation.rotation, this->m_transformation.translation };
}

TransformationMatrix Object::getWorldTransform() const {
    if (this->m_parent) {
        return this->m_parent->getWorldTransform() * this->m_transformation;
    }
    return getLocalTransform();
} // Recursively transform the child by all the adults transformations to go from local to world position

// Setters
void Object::setMaterial(std::string material) {
    if (!materialDatabase.contains(material)) {
        throw std::runtime_error("Material \"" + material + "\" is not in the material database.");
    }
    this->m_material = std::move(material);
}

// Hierarchy
Vector<3> Object::localToWorld(const Vector<3>& localPoint) const {
    return getWorldTransform() * localPoint;
}
Vector<3> Object::worldToLocal(const Vector<3>& worldPoint) const {
    return getWorldTransform().inverse() * worldPoint;
}

// non-const version delegates to const version, removing constness safely
Object* Object::findObjectContainingPoint(const Vector<3>& worldPoint) {
    return const_cast<Object*>(
        std::as_const(*this).findObjectContainingPoint(worldPoint)
    );
}

const Object* Object::findObjectContainingPoint(const Vector<3>& worldPoint) const {
    if (containsPoint(worldPoint)) {
        for (const auto& child : m_children) {
            if (const auto* found = child->findObjectContainingPoint(worldPoint)) {
                return found;
            }
        }
        return this;
    }
    return nullptr;
}


void Object::printHierarchy(const int indent) const {
    std::cout << describeSelf(indent);
    for (const auto& child : this->m_children) {
        child->printHierarchy(indent + 2);
    }
}
