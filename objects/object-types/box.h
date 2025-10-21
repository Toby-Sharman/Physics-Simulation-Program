//
// Created by Tobias Sharman on 03/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_BOX_H
#define PHYSICS_SIMULATION_PROGRAM_BOX_H

#include "../object.h"
#include "vector.h"

class Box final : public Object {
    public:

        template<typename... Args>
        explicit Box(Args&&... args) {
            (setTag(std::forward<Args>(args)), ...);

            attributeAssignmentCheck<Args...>();
        }

        Vector<3> getSize() const { return this->m_size; };
        void setSize(const Vector<3> &size) { this->m_size = size; };

        [[nodiscard]] bool contains(const Vector<3>& worldPoint) const override;
        void print(std::size_t indent) const override;


    private:
        Vector<3> m_size = Vector<3>({0, 0, 0}, "m");
        void setTag(SizeTag<Vector<3>>&& tag) { setSize(tag.value); }

        template<typename T>
        void setTag(T&& tag) { Object::setTag(std::forward<T>(tag)); }
};

#endif //PHYSICS_SIMULATION_PROGRAM_BOX_H