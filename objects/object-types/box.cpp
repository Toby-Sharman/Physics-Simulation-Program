//
// Created by Tobias Sharman on 03/09/2025.
//

#include "box.h"

#include "quantity.h"

#include <iostream>
#include <format>
#include <cmath>

void Box::setSize(const Vector<3> &size) {
    for (auto dimension : size) {
        if (dimension.unit != Unit(1,0,0,0,0,0,0)) {
            throw std::invalid_argument("Size components must be of length dimensions");
        }
    }
    this->m_size = size;
}

bool Box::contains(const Vector<3>& worldPoint) const {

    auto local = worldToLocalPoint(worldPoint);

    return local[0].abs() <= this->m_size[0]/2 &&
           local[1].abs() <= this->m_size[1]/2 &&
           local[2].abs() <= this->m_size[2]/2;
}

void Box::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, "m");

    std::cout << pad << "Box: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Size: " << m_size << std::endl;
}
