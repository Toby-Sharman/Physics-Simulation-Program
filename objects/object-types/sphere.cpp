//
// Created by Tobias Sharman on 03/09/2025.
//

#include "sphere.h"
#include "vector.h"

bool Sphere::containsPoint(const Vector<3>& worldPoint) const {

    const auto local = worldToLocal(worldPoint);

    return local.length() <= this->m_radius;
}

std::string Sphere::describeSelf(const int indent) const {
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

    line += " " + std::format("| Size = ({})", m_radius.asDouble()) + "\n";

    return line;
}

