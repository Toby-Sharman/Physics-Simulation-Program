//
// Created by Tobias Sharman on 03/09/2025.
//

// TODO: Add remove object functionality
// TODO: Add a cache for world and inverse world transforms to reduce computations

#include "vector.h"
#include "matrix.h"

#include "material_database.h"

#include "object.h"

#include <iostream>
#include <memory> // Do I need this?
#include <string>
#include <format>
#include <stdexcept>

// Getters
const std::string& Object::getType() const { return m_type; }
const std::string& Object::getName() const { return m_name; }
Vector<3> Object::getTranslation() const { return Vector<3>{{m_transformation[0][3], m_transformation[1][3], m_transformation[2][3]}}; }
Matrix<double,3,3> Object::getRotationMatrix() const {
    Matrix<double,3,3> R;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            R[i][j] = m_transformation[i][j];
    return R;
}
const std::string& Object::getMaterial() const { return m_material; }
const std::vector<std::shared_ptr<Object>>& Object::children() const { return m_children; }

Matrix<double,4,4> Object::getLocalTransform() const { return m_transformation; }
Matrix<double,4,4> Object::getWorldTransform() const {
    if (const auto p = m_parent.lock()) {
        return p->getWorldTransform() * m_transformation;
    }
    return m_transformation;
} // Recursively transform the child by all the adults transformations to go from local to world position

// Setters
void Object::setType(const std::string& type) { m_type = type; }
void Object::setName(const std::string& name) { m_name = name; }
void Object::setPosition(const double x, const double y, const double z) { m_transformation[0][3] = x; m_transformation[1][3] = y; m_transformation[2][3] = z; }
void Object::setRotationMatrix(const Matrix<double,3,3>& R) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m_transformation[i][j] = R[i][j];

    // Not necessary but ensures matrix is homogenous
    m_transformation[3][0] = m_transformation[3][1] = m_transformation[3][2] = 0.0;
    m_transformation[3][3] = 1.0;
}
void Object::setMaterial(std::string material) {
    if (!MaterialDB::contains(material))
        throw std::runtime_error("Material \"" + material + "\" is not in the material database.");
    m_material = std::move(material);
}

// Hierarchy
Vector<3> Object::localToWorld(const Vector<3>& localPoint) const { return getWorldTransform() * localPoint; }
Vector<3> Object::worldToLocal(const Vector<3>& worldPoint) const { return getWorldTransform().inverse() * worldPoint; }

std::shared_ptr<Object> Object::findObjectContainingPoint(const Vector<3>& worldPoint) {
    for (const auto& child : m_children) {
        if (auto found = child->findObjectContainingPoint(worldPoint)) return found;
    }
    if (containsPoint(worldPoint)) return shared_from_this();
    return nullptr;
}

template <typename T>
concept HasSizeDescription = requires(const T &object)
{
    { object.getSizeDescription() } -> std::same_as<std::string>;
};

void Object::describeSelf(const int indent) const {
    const std::string pad(indent, ' ');
    const std::string material = m_material.empty() ? "material unknown" : m_material;
    const auto localPosition = Vector<3>{m_transformation[0][3], m_transformation[1][3], m_transformation[2][3]};
    const auto worldPosition = getWorldTransform() * Vector<3>{0.0,0.0,0.0};

    std::string line = std::format( "{}{} \"{}\" | Material: {} | Local Pos = ({}, {}, {}) | World Pos = ({}, {}, {})",
        pad,
        m_type,
        m_name,
        material,
        localPosition[0].asDouble(), localPosition[1].asDouble(), localPosition[2].asDouble(),
        worldPosition[0].asDouble(), worldPosition[1].asDouble(), worldPosition[2].asDouble()
    );

    // Append size if available (concept-based design)
    if constexpr (HasSizeDescription<std::remove_cvref_t<decltype(*this)>>) {
        line += " " + this->getSizeDescription();
    }

    std::cout << line << "\n";
}

void Object::printHierarchy(const int indent) const {
    describeSelf(indent);
    for (const auto& child : m_children) {
        child->printHierarchy(indent + 2);
    }
}