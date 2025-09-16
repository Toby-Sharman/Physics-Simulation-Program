//
// Created by Tobias Sharman on 03/09/2025.
//

// TODO: Shift DB to json

#ifndef PHYSICS_SIMULATION_PROGRAM_MATERIAL_DATABASE_H
#define PHYSICS_SIMULATION_PROGRAM_MATERIAL_DATABASE_H

#include "base_database.h"

struct MaterialDB : BaseDB<MaterialDB> {
    static double getRelativePermeability(const std::string& material) {
        return getNumericProperty(material, "relativePermeability");
    }
};

#endif //PHYSICS_SIMULATION_PROGRAM_MATERIAL_DATABASE_H
