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

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_H

#include <concepts>
#include <cstddef> // For std::size_t ignore warning
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "core/maths/matrix.h"
#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"
#include "databases/material-data/material_database.h"
#include "objects/utilities/object_initialisation_tags.h"

// Object
//
// Describes the base object class
//
// Notes on initialisation:
//   - Construction is done via a general constructor that calls on all object types
//   - Construction will return a unique pointer for root objects and for any non-root objects a pointer to the object
//         -> This is due to ownership differences for a root and non-root object
//         -> Handling access to objects via pointers helps keep overhead lower so functionality implemented works based
//            on this
//   - Usage of addChildObject() is not recommended as it creates less clarity than just specifying the parent in the
//     construction
//
// Notes on algorithms:
//   - Position and transforms are all about the centre of the parent object
//         -> As such each rotation and position is defined locally
//   - Getters return const references as they should not be edited and has slightly less overhead
//
// Notes on output:
//   - Output of a singular object is done print and for the entire system use printHierarchy from the object you want
//     to print from, i.e. printHierarchy prints object acting on and its children
//   - printHierarchy adds an ident to indicate that an object is a child of the object above in the printed hierarchy
//
// Supported overloads / operations and functions / methods:
//   - Constructor:            addChild<Object type>(), construct<Object type>()
//   - Attach child object:    addChildObject()
//   - Getters:                get_____() (Parent, Children, Name, Position, Rotation, Material, Temperature,
//                                         NumberDensity, RelativePermeability, LocalTransformation,
//                                         WorldTransformation)
//   - Setters:                set_____() (Parent, Name, Position, Rotation, Material, Temperature, NumberDensity,
//                                         RelativePermeability)
//   - To world transform:     localToWorldPoint(), localToWorldDirection()
//   - To local transform:     worldToLocalPoint(), worldToLocalDirection()
//   - Volumeless check:       isVolumeless()
//   - Containment check:      containsPoint()
//   - Locate point:           findObjectContainingPoint()
//   - Compute intersection:   localIntersection(), worldIntersection()
//   - Compute normal:         localNormal()
//   - Print:                  print()
//   - Print hierarchy:        printHierarchy()
//
// Example usage: TODO
//   const auto world = construct<Box>(
//       name("World"),
//       material("vacuum"),
//       size(Vector<3>({100.0, 50.0, 50.0}, "mm"))
//       );
//   const auto cell = world->addChild<Box>(
//       name("Cell"),
//       material("glass"),
//       size(Vector<3>({25.0, 15.0, 15.0}, "mm"))
//       );
//   const auto vapourCell = cell->addChild<Box>(
//       name("Vapour Cell"),
//       material("gas"),
//       size(Vector<3>({3.0, 3.0, 3.0}, "mm"))
//       );
//   const auto collection = world->addChild<Box>(
//       name("Collection"),
//       material("vacuum"),
//       position(Vector<3>({25.0/2 + 37.5/2, 0.0, 0.0}, "mm")),
//       size(Vector<3>({37.5, 50.0, 50.0}, "mm"))
//       );
//   // Print hierarchy
//   world->printHierarchy();
class Object {
    public:
        Object() = default;
        virtual ~Object() = default;

        // See below class for some constructors

        // Constructor for child objects
        template<typename T, typename... Args>
        [[nodiscard]] T* addChild(Args&&... args) {
            static_assert(std::derived_from<T, Object>, "The object type must derive from Object");

            static_assert(!(std::same_as<std::decay_t<Args>, ParentTag> || ...),
                          "addChild<T>() should not receive a parent tag as its parent is implicitly 'this' (the object being acted on)");

            auto object = std::make_unique<T>(std::forward<Args>(args)...);
            T* pointer = object.get();
            object->m_parent = this;
            this->m_children.push_back(std::move(object)); // Transfer ownership
            return pointer;
        }

        // Attach child object method
        //
        // Transfers ownership to parent
        void addChildObject(std::unique_ptr<Object> child) noexcept {
            child->m_parent = this;
            m_children.push_back(std::move(child));
        }

        // Getters
        [[nodiscard]] constexpr const Object* getParent() const noexcept { return this->m_parent; }
        [[nodiscard]] constexpr const std::vector<std::unique_ptr<Object>>& getChildren() const noexcept { return this->m_children; }
        [[nodiscard]] constexpr const std::string& getName() const noexcept { return this->m_name; }
        [[nodiscard]] constexpr const Vector<3>& getPosition() const noexcept { return this->m_transformation.translation; }
        [[nodiscard]] constexpr const Matrix<3,3>& getRotation() const noexcept { return this->m_transformation.rotation; }
        // Must implement getSize() but hard to enforce with the variant input type to the best of my ability
        [[nodiscard]] constexpr const std::string& getMaterial() const noexcept { return this->m_material; }
        [[nodiscard]] constexpr const Quantity& getTemperature() const noexcept { return this->m_temperature; }
        [[nodiscard]] constexpr const Quantity& getNumberDensity() const noexcept { return this->m_numberDensity; }
        [[nodiscard]] constexpr const double& getRelativePermeability() const noexcept { return this->m_relativePermeability; }
        [[nodiscard]] constexpr TransformationMatrix getLocalTransformation() const noexcept { return this->m_transformation; }
        [[nodiscard]] TransformationMatrix getWorldTransformation() const noexcept; // Recursive combination of transformations

        // Setters
        constexpr void setParent(Object* parent) noexcept { this->m_parent = parent; }
        constexpr void setName(std::string name) noexcept { this->m_name = std::move(name); }
        void setPosition(const Vector<3>& position); // Dimension enforcement
        void setRotation(const Matrix<3,3>& rotation); // Dimension enforcement
        // Must implement setSize() but hard to enforce with the variant input type to the best of my ability
        constexpr void setMaterial(std::string material) noexcept { this->m_material = std::move(material); }
        void setTemperature(Quantity temperature); // Dimension enforcement
        void setNumberDensity(Quantity numberDensity); // Dimension enforcement
        constexpr void setRelativePermeability(const double relativePermeability) noexcept { this->m_relativePermeability = relativePermeability; }

        // To world transform method
        //
        // Transform Cartesian coordinates from local space to world space
        [[nodiscard]] Vector<3> localToWorldPoint(const Vector<3>& localPoint) const noexcept;

        // To local transform method
        //
        // Transform Cartesian coordinates from world space to local space method
        [[nodiscard]] Vector<3> worldToLocalPoint(const Vector<3>& worldPoint) const noexcept;

        // To world transform method
        //
        // Transform Cartesian direction from local space to world space
        [[nodiscard]] Vector<3> localToWorldDirection(const Vector<3>& localDirection) const noexcept;

        // To local transform method
        //
        // Transform Cartesian direction from world space to local space method
        [[nodiscard]] Vector<3> worldToLocalDirection(const Vector<3>& worldDirection) const noexcept;

        // No volume check method
        //
        // Check whether an object has no volume; used to short-circuit containment
        [[nodiscard]] virtual bool isVolumeless() const noexcept = 0;

        // Containment check method
        //
        // Check a point is within an object
        [[nodiscard]] virtual bool contains(const Vector<3>& worldPoint) const = 0;

        // Locate point method
        //
        // Locates an object a point is in within the object tree
        [[nodiscard]] const Object* findObjectContaining(const Vector<3>& worldPoint) const noexcept;

        // Local intersection method
        //
        // Compute an intersection in local coordinates given a start point and displacement
        [[nodiscard]] virtual Vector<3> localIntersection(const Vector<3>& startLocalPoint, const Vector<3>& localDisplacement) const = 0;

        // World intersection method
        //
        // Compute an intersection in world coordinates given a start point and displacement
        [[nodiscard]] Vector<3> worldIntersection(const Vector<3>& startWorldPoint, const Vector<3>& worldDisplacement) const;

        // Local normal method
        //
        // Compute a normal in local coordinates at a specified coordinate
        [[nodiscard]] virtual Vector<3> localNormal(const Vector<3>& localPoint) const = 0;

        // Print method
        //
        // Prints a single object
        virtual void print(std::size_t indent) const = 0;

        // Print hierarchy method
        //
        // Prints the tree of objects about the object acting on, i.e. prints self and all descendants in the tree
        //
        // For further down the tree there is a greater indent
        void printHierarchy(std::size_t indent = 0) const;

    protected:
        Object* m_parent = nullptr;
        std::vector<std::unique_ptr<Object>> m_children;
        std::string m_name = "Unknown";
        TransformationMatrix m_transformation; // Has default values from definition of TransformationMatrix
        std::string m_material = "Unknown";
        Quantity m_temperature = Quantity(293, Unit(0,0,0,0,1,0,0)); // Room temperature
        Quantity m_numberDensity;
        double m_relativePermeability = 1; // Will be set via construction; this is to supress linters or IDEs

        // Tag setters
        //
        // Based on the tag used (as described in object_initialisation_tags.h) set the corresponding attribute
        void setTag(ParentTag&& tag) noexcept { setParent(tag.parent); }
        void setTag(NameTag&& tag) { setName(tag.value); }
        void setTag(PositionTag&& tag) { setPosition(tag.value); }
        void setTag(RotationTag&& tag) { setRotation(tag.value); }
        void setTag(MaterialTag&& tag) { setMaterial(tag.value); }
        void setTag(TemperatureTag&& tag) { setTemperature(tag.value); }
        void setTag(NumberDensityTag&& tag) { setNumberDensity(tag.value); }
        void setTag(RelativePermeabilityTag&& tag) noexcept { setRelativePermeability(tag.value); }

        // Attribute assignment checker method
        //
        // Checks if required fields are assigned
        template<typename... Args>
        void attributeAssignmentCheck() {
            // Check if a material was defined
            if constexpr ((std::same_as<std::decay_t<Args>, MaterialTag> || ...)) {
                if (!materialDatabase.contains(m_material)) {
                    throw std::runtime_error(std::format(
                        "Object '{}' references unknown material '{}' in the material database",
                        m_name,
                        m_material
                    ));
                }

                // If number density wasn't given set from the material database
                if constexpr (!((std::same_as<std::decay_t<Args>, NumberDensityTag> || ...))) {
                    setNumberDensity(materialDatabase.getNumberDensity(m_material));
                }

                // If relative permeability wasn't given set from the material database
                if constexpr (!((std::same_as<std::decay_t<Args>, RelativePermeabilityTag> || ...))) {
                    setRelativePermeability(materialDatabase.getRelativePermeability(m_material));
                }
            } else {
                std::string missing;
                const auto appendMissing = [&](const std::string_view field) {
                    if (!missing.empty()) {
                        missing += " and ";
                    }
                    missing += field;
                };

                // If number density didn't have a way to set
                if constexpr (!((std::same_as<std::decay_t<Args>, NumberDensityTag> || ...))) {
                    appendMissing("NumberDensity");
                }

                // If relative permeability didn't have a way to set
                if constexpr (!((std::same_as<std::decay_t<Args>, RelativePermeabilityTag> || ...))) {
                    appendMissing("RelativePermeability");
                }

                if (!missing.empty()) {
                    throw std::invalid_argument(std::format(
                        "Cannot initialize object '{}' because required attributes ({}) were not provided and no MaterialTag overrides them",
                        m_name,
                        missing
                    ));
                }
            }
        }
};

// Constructor for root objects
template<typename T, typename... Args>
requires (!(std::same_as<std::decay_t<Args>, ParentTag> || ...)) // For if parent is non-specified create root
[[nodiscard]] std::unique_ptr<T> construct(Args&&... args) {
    static_assert(std::derived_from<T, Object>, "The object type must derive from Object");
    return std::make_unique<T>(std::forward<Args>(args)...);
}

// Constructor for child objects
template<typename T, typename... Args>
requires ((std::same_as<std::decay_t<Args>, ParentTag> || ...)) // For is parent is specified create child
[[nodiscard]] T* construct(Args&&... args) {
    static_assert(std::derived_from<T, Object>, "The object type must derive from Object");

    auto object = std::make_unique<T>(std::forward<Args>(args)...);

    T* pointer = object.get();
    object.getParent()->addChildObject(std::move(object)); // Transfer ownership
    return pointer;
}

#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_H