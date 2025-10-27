//
// Created by Tobias Sharman on 03/09/2025.
//

#include "sphere.h"
#include "vector.h"

void Sphere::setSize(const Quantity& radius) {
    if (radius.unit != Unit(1,0,0,0,0,0,0)) {
        throw std::invalid_argument("Size components must be of length dimensions");
    }
    this->m_radius = radius;
}

bool Sphere::contains(const Vector<3>& worldPoint) const {

    const auto local = worldToLocalPoint(worldPoint);

    return local.length() <= this->m_radius;
}

void Sphere::print(const std::size_t indent) const {
    const std::string pad(indent, ' ');
    const auto localPosition = m_transformation.translation;
    const auto worldPosition = getWorldTransformation() * Vector<3>({0.0, 0.0, 0.0}, "m");

    std::cout << pad << "Box: " << m_name << " | Material: " << m_material << " | Local Position: " << localPosition << " | World Position: " << worldPosition << " | Radius: " << m_radius << std::endl;
}

