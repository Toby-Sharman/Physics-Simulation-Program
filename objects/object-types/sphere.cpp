//
// Created by Tobias Sharman on 03/09/2025.
//

#include "sphere.h"
#include "vector.h"

bool Sphere::contains(const Vector<3>& worldPoint) const {

    const auto local = worldToLocal(worldPoint);

    return local.length() <= this->m_radius;
}

void Sphere::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, "m");

    std::cout << pad << "Box: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Radius: " << m_radius << std::endl;
}

