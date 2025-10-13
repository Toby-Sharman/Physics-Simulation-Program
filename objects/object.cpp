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
const std::string& Object::getName() const { return m_name; }
Vector<3> Object::getPosition() const { return m_transformation.translation; }
Matrix<3,3> Object::getRotationMatrix() const { return m_transformation.rotation; }
const std::string& Object::getMaterial() const { return m_material; }
const std::vector<std::shared_ptr<Object>>& Object::children() const { return m_children; }

TransformationMatrix Object::getLocalTransform() const {
    return { m_transformation.rotation, m_transformation.translation };
}

TransformationMatrix Object::getWorldTransform() const {
    if (const auto p = m_parent.lock()) {
        return p->getWorldTransform() * m_transformation;
    }
    return getLocalTransform();
} // Recursively transform the child by all the adults transformations to go from local to world position

// Setters
void Object::setName(const std::string &name) { m_name = name; }
void Object::setPosition(const Vector<3> &position) { m_transformation.translation = position; }
void Object::setRotationMatrix(const Matrix<3,3> &rotation) { m_transformation.rotation = rotation; }
void Object::setMaterial(std::string material) {
    if (!materialDatabase.contains(material)) {
        throw std::runtime_error("Material \"" + material + "\" is not in the material database.");
    }
    m_material = std::move(material);
}

// Hierarchy
Vector<3> Object::localToWorld(const Vector<3>& localPoint) const {
    return getWorldTransform() * localPoint;
}
Vector<3> Object::worldToLocal(const Vector<3>& worldPoint) const {
    return getWorldTransform().inverse() * worldPoint;
}

std::shared_ptr<Object> Object::findObjectContainingPoint(const Vector<3>& worldPoint) {
    for (const auto& child : m_children) {
        if (auto found = child->findObjectContainingPoint(worldPoint)) {
            return found;
        }
    }
    if (containsPoint(worldPoint)) {
        return shared_from_this();
    }
    return nullptr;
}

template <typename T>
concept HasSizeDescription = requires(const T &object)
{
    { object.getSizeDescription() } -> std::same_as<std::string>;
};

void Object::printHierarchy(const int indent) const {
    std::cout << describeSelf(indent);
    for (const auto& child : m_children) {
        child->printHierarchy(indent + 2);
    }
}
