//
// Physics Simulation Program
// File: boundary_interactions.h
// Created by Tobias Sharman on 13/11/2025
//
// Description:
//   - Describes boundary conditions and response for particles
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_BOUNDARY_INTERACTIONS_H
#define PHYSICS_SIMULATION_PROGRAM_BOUNDARY_INTERACTIONS_H

#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"
#include "objects/object.h"
#include "particles/particle.h"

struct BoundaryEvent {
    const Object* surface = nullptr;
    Vector<3> intersection;
};

bool particleBoundaryConditions(const Object* world,
    const Object* startMedium,
    const Vector<3>& startPosition,
    Vector<3>& displacement,
    Quantity& dt,
    BoundaryEvent& event);

void processBoundaryResponse(Particle& particle,
    const BoundaryEvent& event,
    const Vector<3>& eventDisplacement,
    const Quantity& travelledDistance);

#endif //PHYSICS_SIMULATION_PROGRAM_BOUNDARY_INTERACTIONS_H