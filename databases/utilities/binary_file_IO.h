//
// Created by Tobias Sharman on 13/09/2025.
//

#ifndef SIMULATION_PROGRAM_BINARY_FILE_IO_H
#define SIMULATION_PROGRAM_BINARY_FILE_IO_H
#include <fstream>
#include <string>
#include <cstdint>
#include <type_traits>

// Enum (or include from a separate header)
enum class PropertyType : uint8_t {
    BOOL = 0,
    INT = 1,
    DOUBLE = 2,
    STRING = 3
};

namespace BinaryIO {

    template<typename T>
    void write(std::ofstream& out, const T& value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template<typename T>
    bool read(std::ifstream& in, T& value) {
        in.read(reinterpret_cast<char*>(&value), sizeof(T));
        return static_cast<bool>(in);
    }

    inline void writeString(std::ofstream& out, const std::string& str) {
        const auto len = static_cast<uint32_t>(str.size());
        write(out, len);
        out.write(str.data(), len);
    }

    inline bool readString(std::ifstream& in, std::string& str) {
        uint32_t len;
        if (!read(in, len)) return false;
        str.resize(len);
        in.read(str.data(), len);
        return static_cast<bool>(in);
    }

    // For property type section of binary
    template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
    void writeEnum(std::ofstream& out, Enum value) {
        using UT = std::underlying_type_t<Enum>;
        write<UT>(out, static_cast<UT>(value));
    }

    template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
    bool readEnum(std::ifstream& in, Enum& value) {
        using UT = std::underlying_type_t<Enum>;
        UT tmp;
        if (!read<UT>(in, tmp)) return false;
        value = static_cast<Enum>(tmp);
        return true;
    }

}

#endif //SIMULATION_PROGRAM_BINARY_FILE_IO_H