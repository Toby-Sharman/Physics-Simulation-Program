//
// Created by Tobias Sharman on 10/09/2025.
//

#include "field_solver.h"
#include "globals.h"
#include "material_database.h"
#include "vector.h"

auto globalB = Vector<3>{{0.0, 0.0, 1.0},"T"};
auto globalH = globalB / Globals::Constant::Physics::mu0;

Vector<3> getFieldAtPoint(const Vector<3>& point, const std::shared_ptr<Object>& root) {
    if (!root) {
        return globalB;
    }
    const auto obj = root->findObjectContaining(point);
    if (!obj) {
        return globalB;
    }
    return globalB * materialDatabase.getRelativePermeability(obj->getMaterial());
}