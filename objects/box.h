//
// Created by Tobias Sharman on 03/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_BOX_H
#define PHYSICS_SIMULATION_PROGRAM_BOX_H

#include "object.h"
#include "vector.h"

class Box final : public Object {
public:
    void setSizeFromParameters(const Vector<3>& dimensions);
    bool containsPoint(const Vector<3>& world_point) const override;

protected:
    std::string getSizeDescription() const override;


private:
    Vector<3> m_size;  // Using your Vector<3> class
    // double bounding_radius_{0.0f};
};

#endif //PHYSICS_SIMULATION_PROGRAM_BOX_H