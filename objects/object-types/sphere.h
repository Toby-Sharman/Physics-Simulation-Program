//
// Physics Simulation Program
// File: sphere.h
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Describes a sphere object
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//
#ifndef PHYSICS_SIMULATION_PROGRAM_SPHERE_H
#define PHYSICS_SIMULATION_PROGRAM_SPHERE_H

#include "core/linear-algebra/vector.h"
#include "objects/object.h"

// Sphere
//
// Describes implementation of sphere objects. For description see general object class
//
// Spheres are described by a radius and are centred such that the radius is extended in every direction from the
// position defined at initialisation
class Sphere final : public Object {
    public:
        // Constructor for sphere type
        //
        // Note: Not intended for actual use as all functions that use objects work on pointers
        template<typename... Args>
        explicit Sphere(Args&&... args) {
            (setTag(std::forward<Args>(args)), ...); // Use tags defined in class and in parent class

            attributeAssignmentCheck<Args...>(); // Validate object construction is valid
        }

        // Getters and setters for class specific attributes
        [[nodiscard]] constexpr const Quantity& getRadius() const noexcept { return this->m_radius; }
        [[nodiscard]] constexpr const Quantity& getSize() const noexcept { return getRadius(); }
        void setRadius(const Quantity& radius); // Dimension enforcement
        void setSize(const Quantity& radius) { setRadius(radius); }

        // No volume check method
        //
        // Check whether an object has no volume; used to short-circuit containment
        [[nodiscard]] bool isVolumeless() const noexcept override;

        // Containment check method
        //
        // Check a point is within an object
        [[nodiscard]] bool contains(const Vector<3>& worldPoint) const override;

        // Local intersection method
        //
        // Compute an intersection in local coordinates given a start point and displacement
        [[nodiscard]] Vector<3> localIntersection(const Vector<3>& startLocalPoint, const Vector<3>& localDisplacement) const override;

        // Local normal method
        //
        // Compute a normal in local coordinates at a specified coordinate
        [[nodiscard]] Vector<3> localNormal(const Vector<3> &localPoint) const override;

        // Print method
        //
        // Prints a single object
        void print(std::size_t indent) const override;

    private:
        Quantity m_radius = Quantity(0, Unit(1,0,0,0,0,0,0)); // Default is point-like
        void setTag(SizeTag<Quantity>&& tag) { setRadius(tag.value); }

        // Doesn't automatically use inherited tag setters so use this
        template<typename T>
        void setTag(T&& tag) { Object::setTag(std::forward<T>(tag)); }
};

#endif //PHYSICS_SIMULATION_PROGRAM_SPHERE_H