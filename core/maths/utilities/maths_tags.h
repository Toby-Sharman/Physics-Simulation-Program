//
// Created by Tobias Sharman on 10/09/2025.
//

#ifndef SIMULATION_PROGRAM_MATHS_TAGS_H
#define SIMULATION_PROGRAM_MATHS_TAGS_H

#include <vector>
#include <string>

template<typename T> struct Data {
    std::vector<T> values;
    Data(std::initializer_list<T> list) : values(list) {}
};

struct Labels {
    std::vector<std::string> values;
    Labels(const std::initializer_list<std::string> list) : values(list) {}
};

struct Units {
    std::vector<std::string> values;
    Units(const std::initializer_list<std::string> list) : values(list) {}
};

#endif //SIMULATION_PROGRAM_MATHS_TAGS_H