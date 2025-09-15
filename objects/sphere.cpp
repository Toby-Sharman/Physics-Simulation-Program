// //
// // Created by Tobias Sharman on 03/09/2025.
// //
//
// #include "Sphere.h"
//
// #include <iostream>
// #include <cmath>
//
// Sphere::Sphere(const std::string& name) : Object(name, "Sphere") {}
//
// void Sphere::set_size_from_params(const Vector<3>& dimensions) {
//     radius_[0] = dimensions[0];
//     // bounding_radius_ = radius_[0];
// }
//
// bool Sphere::contains_point(const Vector<3>& world_point) const {
//     Vector<3> local = world_to_local(world_point);
//     double dist2 = local[0]*local[0] + local[1]*local[1] + local[2]*local[2];
//     return dist2 <= radius_[0]*radius_[0];
// }
//
// void Sphere::describe_self(int indent) const {
//     std::string pad(indent,' ');
//     std::cout << pad << "Sphere \"" << name_ << "\" radius=" << radius_[0] << "\n";
// }
//
