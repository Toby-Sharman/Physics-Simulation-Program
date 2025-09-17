//
// Created by Tobias Sharman on 03/09/2025.
//

#include "box.h"

#include "quantity.h"

#include <iostream>
#include <format>
#include <cmath>

void Box::setSizeFromParameters(const Vector<3>& dimensions) {
    m_size = Vector<3>{dimensions[0], dimensions[1], dimensions[2]};
    // bounding_radius_ = std::sqrt(size_[0]*size_[0] + size_[1]*size_[1] + size_[2]*size_[2])/2.0f;
}

bool Box::containsPoint(const Vector<3>& world_point) const {
    Vector<3> local = worldToLocal(world_point);
    return std::abs(local[0].asDouble()) <= m_size[0].asDouble()/2 &&
           std::abs(local[1].asDouble()) <= m_size[1].asDouble()/2 &&
           std::abs(local[2].asDouble()) <= m_size[2].asDouble()/2;
}

std::string Box::getSizeDescription() const {
    return std::format("| Size = ({}, {}, {})", m_size[0].asDouble(), m_size[1].asDouble(), m_size[2].asDouble());
}