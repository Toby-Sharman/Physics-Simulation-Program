//
// Created by Tobias Sharman on 08/09/2025.
//

#ifndef SIMULATION_PROGRAM_POINT_H
#define SIMULATION_PROGRAM_POINT_H

#include "vector.h"

#include "object.h"

class Point : public Object {
    public:

    // void set_size_from_params(const Vector<1>& dimensions);
    // bool contains_point(const Vector<3>& world_point) const override;

    protected:
    std::string getSizeDescription() const override;

    private:
    Vector<1> size_ = Vector<1>{{0}};
    Vector<3> position_;
    // double bounding_radius_{0.0f};
};

#endif //SIMULATION_PROGRAM_POINT_H