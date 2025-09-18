//
// Created by Tobias Sharman on 16/09/2025.
//

#ifndef QUANTITY_H
#define QUANTITY_H

#include <string>
#include <iostream>

// Quantity struct
struct Quantity {
    double value;
    std::string unit;
    [[nodiscard]] double asDouble() const { return value; }
    [[nodiscard]] std::string asUnit() const { return unit; }
};

// Non-member operator<< TODO: Format library support
inline std::ostream& operator<<(std::ostream& os, const Quantity& q) {
    os << q.value << " " << q.unit;
    return os;
}

#endif //QUANTITY_H