//
// Created by Tobias Sharman on 03/09/2025.
//

#include "box.h"

#include "quantity.h"

#include <iostream>
#include <format>
#include <cmath>

bool Box::containsPoint(const Vector<3>& worldPoint) const {

    auto local = worldToLocal(worldPoint);

    return local[0].abs() <= this->m_size[0]/2 &&
           local[1].abs() <= this->m_size[1]/2 &&
           local[2].abs() <= this->m_size[2]/2;
}

std::string Box::describeSelf(const int indent) const {
    const std::string pad(indent, ' ');
    const auto material = m_material.empty() ? "material unknown" : m_material;
    const auto localPosition = m_transformation.translation;
    auto worldPositionM = getWorldTransform();
    auto worldPosition = worldPositionM * Vector<3>({0.0, 0.0, 0.0}, "m");

    auto line = std::format( "{}Box: \"{}\" | Material: {} | Local Pos = ({}, {}, {}) | World Pos = ({}, {}, {})",
                             pad,
                             m_name,
                             material,
                             localPosition[0].asDouble(), localPosition[1].asDouble(), localPosition[2].asDouble(),
                             worldPosition[0].asDouble(), worldPosition[1].asDouble(), worldPosition[2].asDouble()
    );

    line += " " + std::format("| Size = ({}, {}, {})", m_size[0].asDouble(), m_size[1].asDouble(), m_size[2].asDouble()) + "\n";

    return line;
}
