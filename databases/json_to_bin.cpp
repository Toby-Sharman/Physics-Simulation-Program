//
// Physics Simulation Program
// File: json_to_bin.cpp
// Created by Tobias Sharman on 13/09/2025
//
// Description:
//   - Convert json files into a binary format
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

// Binary file layout: (Indenting, blank lines, and ellipses means nothing they only help with clarity)
//   [ uint16_t numberOfUnits ]            // numberOfUnits = 2 bytes
//   [ Unit 0 ]                            // serialized Unit object = 7 bytes
//   [ Unit 1 ]
//   ...
//   [ Unit (numberOfUnits-1) ]
//
//   [ Entry 0 Name length ]               // length = 2 bytes
//   [ Entry 0 Name ]                      // string format
//   [ uint32_t Entry 0 propertyCount ]    // propertyCount = 4 bytes
//       [ Property 0 Name length ]        // length = 2 bytes
//       [ Property 0 Name ]               // string format
//       [ Property 0 Type enum ]          // enum (BOOL, INT, DOUBLE, STRING, QUANTITY) = 1 byte
//       [ Property 0 Value ]              // size depends on type:
//       [ Property 1 Name length ]        //   BOOl = 1 byte
//       [ Property 1 Name ]               //   INT = 8 bytes
//       [ Property 1 Type enum ]          //   DOUBLE = 8 bytes
//       [ Property 1 Value ]              //   QUANTITY = 8 (double) + 2 (unit index) = 10 bytes
//       ...                               //   STRING = 2 (length) + length bytes
//
//   [ Entry 1 Name length ]
//   [ Entry 1 Name ]
//   [ uint32_t Entry 1 propertyCount ]
//       [ Property 0 Name length ]
//       [ Property 0 Name ]
//       [ Property 0 Type enum ]
//       [ Property 0 Value ]
//       ...
//
//   ...

#include <cstdint> // For std::uint_t types ignore warning
#include <fstream>
#include <format>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "core/quantities/quantity.h"
#include "core/quantities//utilities/unit_utilities.h"
#include "databases/base_database.h"
#include "databases/utilities/binary_file_IO.h"

using Json = nlohmann::json;

// getPropertyType
//
// Helper function to determine the property type in the json file
[[nodiscard]] static PropertyType getPropertyType(const Json& value) {
    if (value.is_boolean()) {
        return PropertyType::BOOL;
    }
    if (value.is_number_integer()) {
        return PropertyType::INT;
    }
    if (value.is_number_float()) {
        return PropertyType::DOUBLE;
    }
    if (value.is_object() && value.contains("value") && value.contains("unit")) {
        return PropertyType::QUANTITY;
    }
    if (value.is_string()) {
        return PropertyType::STRING;
    }
    throw std::runtime_error(std::format(
        "Unsupported property type '{}'",
        value.type_name()
    ));
}

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

    Json j;
    in >> j;

    std::ofstream out(argv[2], std::ios::binary);
    if (!out) {
        std::cerr << "Error: cannot open output binary file " << argv[2] << "\n";
        return 1;
    }

    // Collect unique Units (by base dimensions)
    std::map<Unit, std::uint16_t> unitIndexMap; // uint8_t (255) would do, but 1 byte extra per file to futureproof is fine
    std::vector<Unit> unitTable;

    // First pass: gather units and normalize values to SI
    std::vector<                 // Entries
        std::pair<
            std::string,         // Entry name
            std::vector<         // List of properties
                std::pair<
                    std::string, // Property name
                    Quantity>    // Value with units
    >>> parsedEntries;

    for (auto& [entryName, properties] : j.items()) {
        if (!properties.is_object()) {
            continue;
        }

        std::vector<std::pair<std::string, Quantity>> entryQuantities;

        for (auto& [propName, value] : properties.items()) {
            if (value.is_object() && value.contains("value") && value.contains("unit")) {
                auto unitString = value["unit"].get<std::string>();
                auto [scale, unit] = parseUnits(unitString);
                double scaledValue = value["value"].get<double>() * scale;

                // Check and insert into map
                if (!unitIndexMap.contains(unit)) {
                    auto idx = static_cast<std::uint16_t>(unitTable.size());
                    unitTable.push_back(unit);
                    unitIndexMap[unit] = idx;
                }

                entryQuantities.emplace_back(propName, Quantity{scaledValue, unit});
            }
        }

        parsedEntries.emplace_back(entryName, std::move(entryQuantities));
    }

    // Write unit table
    auto numUnits = static_cast<std::uint16_t>(unitTable.size());
    BinaryIO::write(out, numUnits);
    for (const auto& u : unitTable) {
        BinaryIO::write(out, u);
    }

    // Write entries
    for (auto& [entryName, properties] : j.items()) {
        if (!properties.is_object()) {
            std::cerr << "Error: entry '" << entryName << "' is not a JSON object\n";
            return 1;
        }

        BinaryIO::writeString(out, entryName);

        auto propertyCount = static_cast<std::uint32_t>(properties.size()); // Kept large for future save state feature
        BinaryIO::write(out, propertyCount);

        for (auto& [propertyName, value] : properties.items()) {
            BinaryIO::writeString(out, propertyName);

            switch (getPropertyType(value)) {
                case PropertyType::BOOL : {
                    BinaryIO::writeEnum(out, PropertyType::BOOL);
                    BinaryIO::write(out, static_cast<std::uint8_t>(value.get<bool>()));
                    break;
                }
                case PropertyType::INT : {
                    BinaryIO::writeEnum(out, PropertyType::INT);
                    BinaryIO::write(out, value.get<int64_t>());
                    break;
                }
                case PropertyType::DOUBLE : {
                    BinaryIO::writeEnum(out, PropertyType::DOUBLE);
                    BinaryIO::write(out, value.get<double>());
                    break;
                }
                case PropertyType::QUANTITY : {
                    BinaryIO::writeEnum(out, PropertyType::QUANTITY);
                    double val = value["value"].get<double>();
                    auto unitString = value["unit"].get<std::string>();
                    auto [factor, unit] = parseUnits(unitString);

                    BinaryIO::write(out, val * factor);
                    auto it = unitIndexMap.find(unit);
                    if (it == unitIndexMap.end()) {
                        std::cerr << "Error: unknown unit '" << unitString << "'\n";
                        return 1;
                    }
                    std::uint16_t idx = it->second;
                    BinaryIO::write(out, idx);
                    break;
                }
                case PropertyType::STRING : {
                    BinaryIO::writeEnum(out, PropertyType::STRING);
                    BinaryIO::writeString(out, value.get<std::string>());
                    break;
                }
                default:
                    std::cerr << "Error: unsupported property type '" << propertyName
                              << "' in entry '" << entryName << "'\n";
                    return 1;
            }
        }
    }

    std::cout << "Converted " << argv[1] << " -> " << argv[2] << "\n";
    return 0;
}