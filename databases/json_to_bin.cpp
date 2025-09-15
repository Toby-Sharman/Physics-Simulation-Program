//
// json_to_bin.cpp
// Converts JSON database to binary format
//

#include "binary_file_IO.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>

using json = nlohmann::json;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: json_to_bin <input.json> <output.bin>\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "Error: cannot open input JSON file " << argv[1] << "\n";
        return 1;
    }

    json j;
    in >> j;

    std::ofstream out(argv[2], std::ios::binary);
    if (!out) {
        std::cerr << "Error: cannot open output binary file " << argv[2] << "\n";
        return 1;
    }

    for (auto& [entryName, properties] : j.items()) {
        BinaryIO::writeString(out, entryName);

        if (!properties.is_object()) {
            std::cerr << "Error: entry '" << entryName << "' is not an object.\n";
            return 1;
        }

        auto propCount = static_cast<uint32_t>(properties.size());
        BinaryIO::write(out, propCount);

        for (auto& [propName, value] : properties.items()) {
            BinaryIO::writeString(out, propName);

            if (value.is_boolean()) {
                BinaryIO::writeEnum(out, PropertyType::BOOL);
                BinaryIO::write(out, static_cast<uint8_t>(value.get<bool>()));
            }
            else if (value.is_number_float() || value.is_number_integer()) {
                if (value.is_number_integer()) {
                    BinaryIO::writeEnum(out, PropertyType::INT);
                    BinaryIO::write(out, value.get<int>());
                } else {
                    BinaryIO::writeEnum(out, PropertyType::DOUBLE);
                    BinaryIO::write(out, value.get<double>());
                }
            }
            else if (value.is_string()) {
                BinaryIO::writeEnum(out, PropertyType::STRING);
                BinaryIO::writeString(out, value.get<std::string>());
            }
            else {
                std::cerr << "Error: property '" << propName
                          << "' in entry '" << entryName
                          << "' has unsupported type.\n";
                return 1;
            }
        }
    }

    std::cout << "Converted " << argv[1] << " → " << argv[2] << "\n";
    return 0;
}
