//
// Physics Simulation Program
// File: field_solver.cpp
// Created by Tobias Sharman on 10/09/2025
//
// Description:
//   - Implementation of field_solver.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "Physics/fields/field_solver.h"

#include "core/globals.h"
#include "databases/material-data/material_database.h"
#include "objects/object_manager.h"

auto g_BFieldStrength = Vector<3>{{0.0, 0.0, 1.0},"T"};
auto g_HFieldStrength = g_BFieldStrength / Globals::Constant::Physics::mu0;

Vector<3> getFieldAtPoint(const Vector<3>& point) {
    const auto root = g_objectManager.getActiveWorld();
    if (!root) {
        return g_BFieldStrength;
    }
    const auto obj = root->findObjectContaining(point);
    if (!obj) {
        return g_BFieldStrength;
    }
    return g_BFieldStrength * g_materialDatabase.getRelativePermeability(obj->getMaterial());
}