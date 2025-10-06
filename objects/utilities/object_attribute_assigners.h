//
// Created by Tobias Sharman on 08/09/2025.
//

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_ATTRIBUTE_ASSIGNERS_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_ATTRIBUTE_ASSIGNERS_H

#include "object_tags.h"

struct ObjectConstructionContext {
    std::string name = "Unnamed";
    std::string material;
    Vector<3> position = Vector<3>({0, 0, 0}, "m");
    Matrix<3,3> rotation = Matrix<3,3>::identity();
    std::optional<Vector<3>> size;
};

template <NameArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.name = std::forward<A>(arg).value;
}

template <MaterialArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.material = std::forward<A>(arg).value;
}

template <PositionArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.position = std::forward<A>(arg).value;
}

template <RotationArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.rotation = std::forward<A>(arg).value;
}

template <SizeArg A>
void assignObjectArgs(ObjectConstructionContext& ctx, A&& arg) {
    ctx.size = std::forward<A>(arg).value;
}

#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_ATTRIBUTE_ASSIGNERS_H