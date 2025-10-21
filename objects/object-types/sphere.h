//
// Created by Tobias Sharman on 03/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_SPHERE_H
#define PHYSICS_SIMULATION_PROGRAM_SPHERE_H

#include "vector.h"

#include "object.h"

#include <vector>

class Sphere final : public Object {
    public:
        Quantity getSize() const { return this->m_radius; };
        void setSize(const Quantity& size) { this->m_radius = size; };

        [[nodiscard]] bool contains(const Vector<3>& worldPoint) const override;
        void print(std::size_t indent) const override;


        template<typename... Args>
        explicit Sphere(Args&&... args) {
            (setTag(std::forward<Args>(args)), ...);

            attributeAssignmentCheck<Args...>();
        }


    private:
        Quantity m_radius;
        void setTag(SizeTag<Quantity>&& tag) { setSize(tag.value); }

        template<typename T>
        void setTag(T&& tag) { Object::setTag(std::forward<T>(tag)); }
};

#endif //PHYSICS_SIMULATION_PROGRAM_SPHERE_H