//
// Created by Tobias Sharman on 03/09/2025.
//

// Position and transforms are all about the centre of the above object

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_H

#include "vector.h"
#include "matrix.h"


#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct NameTag { std::string value; };
inline NameTag name(const std::string& s) { return NameTag{s}; }
template <typename T> concept NameArg = std::same_as<std::decay_t<T>, NameTag>;

struct MaterialTag { std::string value; };
inline MaterialTag material(const std::string& s) { return MaterialTag{s}; }
template <typename T> concept MaterialArg = std::same_as<std::decay_t<T>, MaterialTag>;

struct PositionTag { Vector<3> value; };
inline PositionTag position(const Vector<3>& v) { return PositionTag{v}; }
template <typename T> concept PositionArg = std::same_as<std::decay_t<T>, PositionTag>;

struct RotationTag { Matrix<3,3> value; };
inline RotationTag rotation(const Matrix<3,3>& m) { return RotationTag{m}; }
template <typename T> concept RotationArg = std::same_as<std::decay_t<T>, RotationTag>;

struct SizeTag { Vector<3> value; };
inline SizeTag size(const Vector<3>& v) { return SizeTag{v}; }
template <typename T> concept SizeArg = std::same_as<std::decay_t<T>, SizeTag>;

struct ObjectConstructionContext {
    std::string name = "Unnamed";
    std::string material;
    Vector<3> position = Vector<3>({0, 0, 0}, "m");
    Matrix<3,3> rotation = Matrix<3,3>::identity();
    std::optional<Vector<3>> size;
};

template <NameArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.name = std::forward<A>(arg).value;
}

template <MaterialArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.material = std::forward<A>(arg).value;
}

template <PositionArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.position = std::forward<A>(arg).value;
}

template <RotationArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.rotation = std::forward<A>(arg).value;
}

template <SizeArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.size = std::forward<A>(arg).value;
}


class Object : public std::enable_shared_from_this<Object> {
    public:
        Object() = default;
        virtual ~Object() = default;

        // Getters
        const std::string& getName() const;
        Vector<3> getPosition() const;
        Matrix<3, 3> getRotationMatrix() const;
        const std::string& getMaterial() const;
        const std::vector<std::shared_ptr<Object>>& children() const;

        TransformationMatrix getLocalTransform() const;
        TransformationMatrix getWorldTransform() const;

        // Setters
        void setName(const std::string &name);
        void setPosition(const Vector<3> &position);
        void setRotationMatrix(const Matrix<3,3> &rotation);
        void setMaterial(std::string material);

        // Hierarchy
        Vector<3> localToWorld(const Vector<3>& localPoint) const;
        Vector<3> worldToLocal(const Vector<3>& worldPoint) const;

        std::shared_ptr<Object> findObjectContainingPoint(const Vector<3>& worldPoint);
        template<typename T, typename... Args>
        std::shared_ptr<T> addChild(Args&&... args) {
            auto child = construct<T>(std::forward<Args>(args)...); // Constructor is below
            child->m_parent = shared_from_this();
            m_children.push_back(child);
            return child;
        }

        virtual std::string describeSelf(int indent) const { return ""; };
        void printHierarchy(int indent = 0) const;

        // Must be implemented by derived objects
        virtual bool containsPoint(const Vector<3>& world_point) const = 0;
        virtual std::string getSizeDescription() const { return ""; }
        // Set size from params not included as it can take a variety of inputs depending on object type

    protected:
        std::string m_name;
        std::string m_material;
        TransformationMatrix m_transformation;
        std::weak_ptr<Object> m_parent;
        std::vector<std::shared_ptr<Object>> m_children;
};

// Templated helper: create child in place with constructor arguments
template<typename T, typename... Args>
std::shared_ptr<T> construct(Args&&... args) { // TODO: Should first object be a shared pointer
    static_assert(std::is_base_of_v<Object, T>,
                  "construct: T must derive from Object");
    auto object = std::make_shared<T>();
    ObjectConstructionContext ctx;
    (assignObjectArgs(ctx, std::forward<Args>(args)), ...);

    object->setName(ctx.name);
    object->setPosition(ctx.position);
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