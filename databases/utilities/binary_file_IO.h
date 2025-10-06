//
// Physics Simulation Program
// File: binary_file_IO.h
// Created by Tobias Sharman on 13/09/2025
//
// Description:
//   - Describes Binary file IO operations
//         -> Reading and writing
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_BINARY_FILE_IO_H
#define PHYSICS_SIMULATION_PROGRAM_BINARY_FILE_IO_H

#include <cstdint> // For uint32_t ignore warning
#include <fstream>
#include <string>
#include <type_traits>

#include "core/maths/utilities/quantity.h"

// BinaryIO
//
// A namespace containing different binary operations for reading and writing binary files
namespace BinaryIO {

    // Write primitive type to binary stream
    template<typename T>
    void write(std::ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    // Read primitive type from binary stream
    template<typename T>
    bool read(std::ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        return static_cast<bool>(in);
    }

    // Write a string with length prefix
    inline void writeString(std::ofstream& out, const std::string& string) {
        const auto length = static_cast<uint32_t>(string.size());
        write(out, length);
        out.write(string.data(), length);
    }

    // Read a string with length prefix
    inline bool readString(std::ifstream& in, std::string& string) {
        uint32_t length;
        if (!read(in, length)) {
            return false;
        }
        string.resize(length);
        in.read(string.data(), length);
        return static_cast<bool>(in);
    }

    // Write a Quantity (value + 7 unit exponents)
    inline void writeQuantity(std::ofstream& out, const Quantity& quantity) {
        write(out, quantity.value);
        out.write(reinterpret_cast<const char*>(quantity.unit.exponents.data()), 7);
    }

    // Read a Quantity (value + 7 unit exponents)
    inline bool readQuantity(std::ifstream& in, Quantity& quantity) {
        if (!read(in, quantity.value)) {
            return false;
        }
        in.read(reinterpret_cast<char*>(quantity.unit.exponents.data()), 7);
        return static_cast<bool>(in);
    }

    // Write enum as underlying integer type
    //
    // Used to identify type in the following bytes in the binary
    template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
    void writeEnum(std::ofstream& out, Enum value) {
        using UnitType = std::underlying_type_t<Enum>;
        write<UnitType>(out, static_cast<UnitType>(value));
    }

    // Read enum as underlying integer type
    //
    // Used to identify type in the following bytes in the binary
    template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
    bool readEnum(std::ifstream& in, Enum& value) {
        using UnitType = std::underlying_type_t<Enum>;
        UnitType tmp;
        if (!read<UnitType>(in, tmp)) {
            return false;
        }
        value = static_cast<Enum>(tmp);
        return true;
    }

}

#endif //PHYSICS_SIMULATION_PROGRAM_BINARY_FILE_IO_H