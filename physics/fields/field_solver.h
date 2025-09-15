//
// Created by Tobias Sharman on 10/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_FIELD_SOLVER_H
#define PHYSICS_SIMULATION_PROGRAM_FIELD_SOLVER_H

#include "vector.h"

#include "object.h"

extern Vector<3> GLOBAL_B;

Vector<3> getFieldAtPoint(const Vector<3>& point, const std::shared_ptr<Object>& root);

#endif //PHYSICS_SIMULATION_PROGRAM_FIELD_SOLVER_H