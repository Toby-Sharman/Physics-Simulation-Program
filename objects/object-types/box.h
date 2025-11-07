//
// Physics Simulation Program
// File: box.h
// Created by Tobias Sharman on 03/09/2025
//
// Description:
//   - Describes a box type object
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_BOX_H
#define PHYSICS_SIMULATION_PROGRAM_BOX_H

#include "core/linear-algebra/vector.h"
#include "objects/object.h"

// Box
//
// Describes implementation of box objects. For description see general object class
//
// Boxes are described by given dimensions and are centred such that the defined dimensions are half spanned in each
// direction from the position defined at initialisation
class Box final : public Object {
    public:
        // Constructor for box type
        //
        // Note: Not intended for actual use as all functions that use objects work on pointers
        template<typename... Args>
        explicit Box(Args&&... args) {
            (setTag(std::forward<Args>(args)), ...); // Use tags defined in class and in parent class

            attributeAssignmentCheck<Args...>(); // Validate object construction is valid
        }

        // Getters and setters for class specific attributes
        [[nodiscard]] constexpr const Vector<3>& getSize() const noexcept { return this->m_size; };
        void setSize(const Vector<3> &size); // Dimension enforcement

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
        Vector<3> m_size = Vector<3>({0, 0, 0}, Unit(1,0,0,0,0,0,0)); // Default is point-like
        void setTag(SizeTag<Vector<3>>&& tag) { setSize(tag.value); }

        // Doesn't automatically use inherited tag setters so use this
        template<typename T>
        void setTag(T&& tag) { Object::setTag(std::forward<T>(tag)); }
};

#endif //PHYSICS_SIMULATION_PROGRAM_BOX_H