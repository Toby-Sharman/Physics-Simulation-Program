//
// Created by Tobias Sharman on 03/09/2025.
//

// TODO: Shift DB to json

#ifndef SIMULATION_PROGRAM_MATERIAL_DATABASE_H
#define SIMULATION_PROGRAM_MATERIAL_DATABASE_H

#include "base_database.h"

class MaterialDB : public BaseDB<MaterialDB> {
public:
    static double getRelativePermeability(const std::string& material) {
        return getProperty(material, "relativePermeability");
    }
};

#endif //SIMULATION_PROGRAM_MATERIAL_DATABASE_H
