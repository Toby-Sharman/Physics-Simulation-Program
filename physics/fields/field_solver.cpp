//
// Created by Tobias Sharman on 10/09/2025.
//

#include "vector.h"

#include "globals.h"
#include "material_database.h"

#include "field_solver.h"

auto GLOBAL_B = Vector<3>{Data{0.0, 0.0, 1.0}, Labels{"x", "y", "z"}, Units{"T", "T", "T"}};
auto Global_H = GLOBAL_B / Globals::Constant::MU0;

Vector<3> getFieldAtPoint(const Vector<3>& point, const std::shared_ptr<Object>& root) {
    if (!root) return GLOBAL_B;
    const auto obj = root->findObjectContainingPoint(point);
    if (!obj) return GLOBAL_B;
    return GLOBAL_B * MaterialDB::getRelativePermeability(obj->getMaterial());
}