//
// json_to_bin.cpp
// Converts JSON database to binary format with Quantities and unit table
//

#include "binary_file_IO.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>

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

    // -------------------------------
    // Step 1: Collect unique units
    // -------------------------------
    std::unordered_map<std::string, uint16_t> unitIndexMap;
    std::vector<std::string> unitTable;

    for (auto& [entryName, properties] : j.items()) {
        if (!properties.is_object()) continue;

        for (auto& [propName, value] : properties.items()) {
            if (value.is_object() && value.contains("value") && value.contains("unit")) {
                std::string u = value["unit"].get<std::string>();
                if (!unitIndexMap.contains(u)) {
                    auto idx = static_cast<uint16_t>(unitTable.size());
                    unitTable.push_back(u);
                    unitIndexMap[u] = idx;
                }
            }
        }
    }

    // -------------------------------
    // Step 2: Write unit table
    // -------------------------------
    auto numUnits = static_cast<uint16_t>(unitTable.size());
    BinaryIO::write(out, numUnits);
    for (const auto& u : unitTable) {
        BinaryIO::writeString(out, u);
    }

    // -------------------------------
    // Step 3: Write entries and properties
    // -------------------------------
    for (auto& [entryName, properties] : j.items()) {
        if (!properties.is_object()) {
            std::cerr << "Error: entry '" << entryName << "' is not an object.\n";
            return 1;
        }

        BinaryIO::writeString(out, entryName);

        auto propCount = static_cast<uint32_t>(properties.size());
        BinaryIO::write(out, propCount);

        for (auto& [propName, value] : properties.items()) {
            BinaryIO::writeString(out, propName);

            // -------------------
            // Quantity object
            // -------------------
            if (value.is_object() && value.contains("value") && value.contains("unit")) {
                BinaryIO::writeEnum(out, PropertyType::QUANTITY);
                double val = value["value"].get<double>();
                auto unit = value["unit"].get<std::string>();

                BinaryIO::write(out, val);
                auto it = unitIndexMap.find(unit);
                if (it == unitIndexMap.end()) {
                    std::cerr << "Error: unknown unit '" << unit << "'\n";
                    return 1;
                }
                uint16_t idx = it->second;
                BinaryIO::write(out, idx);
            }
            // -------------------
            // Boolean
            // -------------------
            else if (value.is_boolean()) {
                BinaryIO::writeEnum(out, PropertyType::BOOL);
                BinaryIO::write(out, static_cast<uint8_t>(value.get<bool>()));
            }
            // -------------------
            // Integer or double
            // -------------------
            else if (value.is_number_integer()) {
                BinaryIO::writeEnum(out, PropertyType::INT);
                BinaryIO::write(out, value.get<int32_t>());
            }
            else if (value.is_number_float()) {
                BinaryIO::writeEnum(out, PropertyType::DOUBLE);
                BinaryIO::write(out, value.get<double>());
            }
            // -------------------
            // String
            // -------------------
            else if (value.is_string()) {
                BinaryIO::writeEnum(out, PropertyType::STRING);
                BinaryIO::writeString(out, value.get<std::string>());
            }
            else {
                std::cerr << "Error: unsupported property type '" << propName
                          << "' in entry '" << entryName << "'\n";
                return 1;
            }
        }
    }

    std::cout << "Converted " << argv[1] << " → " << argv[2] << "\n";
    return 0;
}
