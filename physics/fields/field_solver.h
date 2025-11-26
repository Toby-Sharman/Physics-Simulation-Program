//
// Physics Simulation Program
// File: field_solver.h
// Created by Tobias Sharman on 10/09/2025
//
// Description:
//   - Helper for getting magnetic field at a point - currently very simply unrealistic implementation
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

// TODO: Implement more realistic fields and base field strength

#ifndef PHYSICS_SIMULATION_PROGRAM_FIELD_SOLVER_H
#define PHYSICS_SIMULATION_PROGRAM_FIELD_SOLVER_H

#include "core/linear-algebra/vector.h"

extern Vector<3> g_BFieldStrength;
extern Vector<3> g_HFieldStrength;

Vector<3> getFieldAtPoint(const Vector<3>& point);

#endif //PHYSICS_SIMULATION_PROGRAM_FIELD_SOLVER_H