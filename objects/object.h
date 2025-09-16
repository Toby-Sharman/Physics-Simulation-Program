//
// Created by Tobias Sharman on 03/09/2025.
//

// Position and transforms are all about the centre of the above object

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_H

#include "vector.h"
#include "matrix.h"

#include "object_types.h"
#include "object_attribute_assigners.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Object : public std::enable_shared_from_this<Object> {
    public:
    Object() = default;
    virtual ~Object() = default;

    // Getters
    const std::string& getName() const;
    const std::string& getType() const;
    Vector<3> getTranslation() const;
    Matrix<double, 3, 3> getRotationMatrix() const;
    const std::string& getMaterial() const;
    const std::vector<std::shared_ptr<Object>>& children() const;

    Matrix<double,4,4> getLocalTransform() const;
    Matrix<double,4,4> getWorldTransform() const;

    // Setters
    void setType(const std::string& type);
    void setName(const std::string& name);
    void setPosition(double x, double y, double z);
    void setRotationMatrix(const Matrix<double,3,3>& R);
    void setMaterial(std::string material);

    // Hierarchy
    Vector<3> localToWorld(const Vector<3>& localPoint) const;
    Vector<3> worldToLocal(const Vector<3>& worldPoint) const;

    std::shared_ptr<Object> findObjectContainingPoint(const Vector<3>& worldPoint);
    template<typename T, typename... Args>
    std::shared_ptr<T> addChild(Args&&... args) {
        auto child = construct<T>(std::forward<Args>(args)...); // Constructor is below
        child->parent_ = shared_from_this();
        m_children.push_back(child);
        return child;
    }

    void describeSelf(int indent) const;
    void printHierarchy(int indent = 0) const;

    // Must be implemented by derived objects
    virtual bool containsPoint(const Vector<3>& world_point) const = 0;
    virtual std::string getSizeDescription() const { return ""; }
    // Set size from params not included as it can take a variety of inputs depending on object type

    protected:
    std::string m_name;
    std::string m_type;
    std::string m_material;
    Matrix<double,4,4> m_transformation;
    std::weak_ptr<Object> m_parent;
    std::vector<std::shared_ptr<Object>> m_children;
};

// Templated helper: create child in place with constructor arguments
template<typename T, typename... Args>
std::shared_ptr<T> construct(Args&&... args) {
    static_assert(std::is_base_of_v<Object, T>,
                  "construct: T must derive from Object");
    auto object = std::make_shared<T>();
    ObjectConstructionContext ctx;
    (assignObjectArgs(ctx, std::forward<Args>(args)), ...);

    object->setType(ObjectTypeName<T>::name);
    object->setName(ctx.name);
    object->setPosition(ctx.position[0], ctx.position[1], ctx.position[2]);
    object->setRotationMatrix(ctx.rotation);
    object->setMaterial(ctx.material);

    // Handle size correctly for all combinations
    if constexpr (requires(T& obj, const Vector<3>& dimensions) { obj.setSizeFromParameters(dimensions); }) {
        // Object supports size
        if (!ctx.size.has_value()) {
            throw std::runtime_error("Size must be specified for this object type");
        }
        object->setSizeFromParameters(*ctx.size);
    } else {
        // Object does not support size
        if (ctx.size.has_value()) {
            throw std::runtime_error("Size provided but not supported by this object type");
        }
    }
    return object;
}


#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_H