//
// Physics Simulation Program
// File: object.h
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Describes a general base object class
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

// Position and transforms are all about the centre of the above object

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_H

#include "core/maths/matrix.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct ParentTag { class Object* parent; };
constexpr ParentTag parent(Object* ptr) { return {ptr}; }
template<typename T>
constexpr ParentTag parent(const std::unique_ptr<T>& object) requires std::derived_from<T, Object> {
    return {object.get()};
}
template<typename T>
concept ParentArgument = std::same_as<std::decay_t<T>, ParentTag>;


struct NameTag { std::string value; };
constexpr NameTag name(const std::string& name) { return {name}; }
template <typename T>
concept NameArgument = std::same_as<std::decay_t<T>, NameTag>;

struct PositionTag { Vector<3> value; };
constexpr PositionTag position(const Vector<3>& position) { return {position}; }
template <typename T>
concept PositionArgument = std::same_as<std::decay_t<T>, PositionTag>;

struct RotationTag { Matrix<3,3> value; };
constexpr RotationTag rotation(const Matrix<3,3>& rotation) { return {rotation}; }
template <typename T>
concept RotationArgument = std::same_as<std::decay_t<T>, RotationTag>;

template <typename T>
struct SizeTag { T value; };
template <typename T>
constexpr SizeTag<T> size(const T& dimensions) { return {dimensions}; }
template <typename T>
concept SizeArgument = requires(T type) { type.value; };

struct MaterialTag { std::string value; };
constexpr MaterialTag material(const std::string& material) { return {material}; }
template <typename T>
concept MaterialArgument = std::same_as<std::decay_t<T>, MaterialTag>;

struct TemperatureTag { Quantity value; };
constexpr TemperatureTag temperature(const Quantity& temperature) { return {temperature}; }
template <typename T>
concept TemperatureArgument = std::same_as<std::decay_t<T>, TemperatureTag>;

class Object {
    public:
        Object() = default;
        virtual ~Object() = default;

        template<typename... Args>
        explicit Object(Args&&... args) { (setTag(std::forward<Args>(args)), ...); }

        template<typename T, typename... Args>
        friend T* construct(Args&&... args);

        // Getters
        [[nodiscard]] const Object* parent() const { return this->m_parent; }
        [[nodiscard]] const std::vector<std::unique_ptr<Object>>& children() const { return this->m_children; }

        [[nodiscard]] const std::string& getName() const { return this->m_name; }
        [[nodiscard]] Vector<3> getPosition() const { return this->m_transformation.translation; }
        [[nodiscard]] Matrix<3, 3> getRotationMatrix() const { return this->m_transformation.rotation; }
        [[nodiscard]] const std::string& getMaterial() const { return this->m_material; }
        [[nodiscard]] const Quantity& getTemperature() const { return this->m_temperature; }

        [[nodiscard]] TransformationMatrix getLocalTransform() const;
        [[nodiscard]] TransformationMatrix getWorldTransform() const;

        // Setters
        void setParent(Object* parent) { this->m_parent = parent; }

        void setName(const std::string& name) { this->m_name = name; }
        void setPosition(const Vector<3>& position) { this->m_transformation.translation = position; }
        void setRotationMatrix(const Matrix<3,3>& rotation) { this->m_transformation.rotation = rotation; }
        void setMaterial(std::string material);
        void setTemperature(const Quantity& temperature) { this->m_temperature = temperature; }

        // Hierarchy
        template<typename T, typename... Args>
        T* addChild(Args&&... args) {
            static_assert(std::derived_from<T, Object>,
              "addChild<T>() requires T to derive from Object");

            static_assert(!(std::same_as<std::decay_t<Args>, ParentTag> || ...),
                          "addChild<T>() should not receive a parent tag as its parent is implicitly 'this' (the object being acted on)");
            auto object = std::make_unique<T>(std::forward<Args>(args)...);
            object->m_parent = this;
            T* pointer = object.get();
            this->m_children.push_back(std::move(object));
            return pointer;
        }

        void addChildObject(std::unique_ptr<Object> child) {
            child->m_parent = this;
            m_children.push_back(std::move(child));
        }

        Vector<3> localToWorld(const Vector<3>& localPoint) const;
        Vector<3> worldToLocal(const Vector<3>& worldPoint) const;

        Object* findObjectContainingPoint(const Vector<3>& worldPoint);
        [[nodiscard]] const Object* findObjectContainingPoint(const Vector<3>& worldPoint) const;

        [[nodiscard]] virtual std::string describeSelf(int indent) const { return ""; };
        void printHierarchy(int indent = 0) const;

        // Must be implemented by derived objects
        [[nodiscard]] virtual bool containsPoint(const Vector<3>& worldPoint) const = 0;

    protected:
        Object* m_parent = nullptr;
        std::vector<std::unique_ptr<Object>> m_children;
        std::string m_name;
        TransformationMatrix m_transformation;
        std::string m_material;
        Quantity m_temperature = Quantity(293, "K");

        // Tag setters
        void setTag(ParentTag&& tag) { setParent(tag.parent); }
        void setTag(NameTag&& tag) { setName(tag.value); }
        void setTag(PositionTag&& tag) { setPosition(tag.value); }
        void setTag(RotationTag&& tag) { setRotationMatrix(tag.value); }
        void setTag(MaterialTag&& tag) { setMaterial(tag.value); }
        void setTag(TemperatureTag&& tag) { setTemperature(tag.value); }

        // Fallback for unknown tags (like SizeTag in base) -> do nothing
        template<typename T>
        static void setTag(T&&) {}
};

template<typename T, typename... Args>
requires ((std::same_as<std::decay_t<Args>, ParentTag> || ...))
T* construct(Args&&... args) {
    static_assert(std::derived_from<T, Object>, "T must derive from Object");

    auto object = std::make_unique<T>(std::forward<Args>(args)...);

    Object* parent = nullptr;
    (([&] {
        if constexpr (std::same_as<std::decay_t<Args>, ParentTag>) {
            parent = args.parent;
        }
    }()), ...);

    T* ptr = object.get();
    parent->addChildObject(std::move(object));
    return ptr;
}

template<typename T, typename... Args>
requires (!(std::same_as<std::decay_t<Args>, ParentTag> || ...))
std::unique_ptr<T> construct(Args&&... args) {
    static_assert(std::derived_from<T, Object>, "T must derive from Object");
    return std::make_unique<T>(std::forward<Args>(args)...);
}


#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_H