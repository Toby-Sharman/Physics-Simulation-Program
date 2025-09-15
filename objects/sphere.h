// //
// // Created by Tobias Sharman on 03/09/2025.
// //
//
// #ifndef SIMULATION_PROGRAM_SPHERE_H
// #define SIMULATION_PROGRAM_SPHERE_H
//
// #include "Vector.h"
//
// #include "Object.h"
//
// #include <vector>
//
// class Sphere : public Object {
// public:
//     Sphere(const std::string& name);
//
//     void set_size_from_params(const Vector<3>& dimensions);
//     virtual bool contains_point(const Vector<3>& world_point) const override;
//
// protected:
//     virtual void describe_self(int indent) const override;
//
// private:
//     Vector<1> radius_{1.0f};  // Using Vector<1> for consistency
//     // double bounding_radius_{0.0f};
// };
//
// #endif //SIMULATION_PROGRAM_SPHERE_H