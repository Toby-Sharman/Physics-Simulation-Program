//
// Created by Tobias Sharman on 16/09/2025.
//

#ifndef QUANTITY_H
#define QUANTITY_H

#include <string>

// Quantity struct
struct Quantity {
    double value = 0.0;
    std::string unit;
    double asDouble() const { return value; }
};

#endif //QUANTITY_H