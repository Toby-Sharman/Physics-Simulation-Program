//
// Created by Tobias Sharman on 10/09/2025.
//

#ifndef SIMULATION_PROGRAM_OBJECT_TYPES_H
#define SIMULATION_PROGRAM_OBJECT_TYPES_H

// Forward declaration should minimally improve compile time
class Box;

template<typename T>
struct ObjectTypeName;

template<>
struct ObjectTypeName<Box> {
    static constexpr auto name = "Box";
};

// template<>
// struct ObjectTypeName<Sphere> {
//     static constexpr const char* name = "Sphere";
// };

// template<>
// struct ObjectTypeName<Point> {
//     static constexpr const char* name = "Point";
// };

#endif //SIMULATION_PROGRAM_OBJECT_TYPES_H