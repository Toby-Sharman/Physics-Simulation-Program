//
// Created by Tobias Sharman on 03/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_BOX_H
#define PHYSICS_SIMULATION_PROGRAM_BOX_H

#include "../object.h"
#include "vector.h"

class Box final : public Object {
    public:
        Vector<3> getSize() const { return this->m_size; };
        void setSize(const Vector<3> &size) { this->m_size = size; };

        [[nodiscard]] bool containsPoint(const Vector<3>& worldPoint) const override;
        [[nodiscard]] std::string describeSelf(int indent) const override;


        template<typename... Args>
        explicit Box(Args&&... args) : Object() {
            (setTag(std::forward<Args>(args)), ...);
        }


    private:
        Vector<3> m_size;
        void setTag(SizeTag<Vector<3>>&& tag) { setSize(tag.value); }

        template<typename T>
        void setTag(T&& tag) { Object::setTag(std::forward<T>(tag)); }
};

#endif //PHYSICS_SIMULATION_PROGRAM_BOX_H