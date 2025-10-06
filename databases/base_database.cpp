//
// Physics Simulation Program
// File: base_database.cpp
// Created by Tobias Sharman on 05/10/2025
//
// Description:
//   - Contains functions for handling binary file databases in my format
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include <algorithm>
#include <cstdint> // For uint types ignore warning
#include <format>
#include <fstream>
#include <map>
#include <ranges>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "databases/base_database.h"
#include "databases/utilities/binary_file_IO.h"

void BaseDatabase::loadFromBinary(const std::string& filepath) {
    m_db.clear(); // Clear previous content to: prevent duplicates if reusing the same binary; or a merge of 2 databases

    std::ifstream in(filepath, std::ios::binary);
    if (!in) {
        throw std::runtime_error(std::format("Cannot open database file '{}'", filepath));
    }

    uint16_t numberOfUnits; // uint8_t (255) would do, but 1 byte extra per file overhead to futureproof is fine
    if (!BinaryIO::read(in, numberOfUnits)) {
        throw std::runtime_error("Failed to read number of units");
    }

    std::vector<Unit> unitTable(numberOfUnits);
    for (uint16_t i = 0; i < numberOfUnits; ++i) {
        BinaryIO::read(in, unitTable[i]);
    }

    while (true) {
        DatabaseEntry entry;

        // Read next entry and assign name to the entry struct
        if (!BinaryIO::readString(in, entry.name)) {
            break;
        }

        uint32_t propertyCount; // Kept large as if wanting to add save state to program 4, 294, 967, 295 is plenty

        // Read next section of file and assign to variable propertyCount
        if (!BinaryIO::read(in, propertyCount)) {
            throw std::runtime_error(
                std::format("Unexpected EOF reading property count for '{}'", entry.name)
                );
        }

        entry.properties.reserve(propertyCount); // Avoid reallocation for large amounts of properties

        for (uint32_t i = 0; i < propertyCount; ++i) {
            DatabaseProperty property;
            if (!BinaryIO::readString(in, property.name)) {
                throw std::runtime_error(
                    std::format("Unexpected EOF reading property name for '{}'", entry.name)
                    );
            }
            if (!BinaryIO::readEnum(in, property.type)) {
                throw std::runtime_error(
                    std::format("Unexpected EOF reading property type for '{}.{}'", entry.name, property.name)
                );
            }

            switch (property.type) {
                case PropertyType::BOOL: {
                    uint8_t boolean;
                    if (!BinaryIO::read(in, boolean)) {
                        throw std::runtime_error(
                            std::format("Unexpected EOF reading bool '{}.{}'", entry.name, property.name)
                        );
                    }
                    property.value = (boolean != 0);
                    break;
                }
                case PropertyType::INT: {
                    int64_t integer;
                    if (!BinaryIO::read(in, integer)) {
                        throw std::runtime_error(
                            std::format("Unexpected EOF reading int '{}.{}'", entry.name, property.name)
                            );
                    }
                    property.value = integer;
                    break;
                }
                case PropertyType::DOUBLE: {
                    double floating;
                    if (!BinaryIO::read(in, floating)) {
                        throw std::runtime_error(
                            std::format("Unexpected EOF reading double '{}.{}'", entry.name, property.name)
                            );
                    }
                    property.value = floating;
                    break;
                }
                case PropertyType::STRING: {
                    std::string string;
                    if (!BinaryIO::readString(in, string)) {
                        throw std::runtime_error(
                            std::format("Unexpected EOF reading string '{}.{}'", entry.name, property.name)
                            );
                    }
                    property.value = string;
                    break;
                }
                case PropertyType::QUANTITY: {
                    Quantity quantity;
                    if (!BinaryIO::read(in, quantity.value)) {
                        throw std::runtime_error(
                            std::format("Unexpected EOF reading Quantity value '{}.{}'", entry.name, property.name)
                            );
                    }
                    uint16_t idx;
                    if (!BinaryIO::read(in, idx) || idx >= unitTable.size()) {
                        throw std::runtime_error(
                            std::format("Invalid unit index for '{}.{}'", entry.name, property.name)
                            );
                    }
                    quantity.unit = unitTable[idx];
                    property.value = quantity;
                    break;
                }
                default:
                    throw std::runtime_error(
                        std::format("Unknown property type for '{}.{}'", entry.name, property.name)
                        );
            }

            entry.properties.push_back(std::move(property));
        }

        m_db.push_back(std::move(entry));
    }
}

void BaseDatabase::saveToBinary(const std::string& filepath) {
    std::ofstream out(filepath, std::ios::binary);
    if (!out) {
        throw std::runtime_error(std::format("Cannot open file '{}'", filepath));
    }

    std::map<Unit, uint16_t> unitIndexMap;
    std::vector<Unit> unitTable;

    for (const auto& [entryName, entryProperties] : m_db) {
        for (const auto& property : entryProperties) {
            if (property.type == PropertyType::QUANTITY) {
                if (const auto& u = std::get<Quantity>(property.value).unit; !unitIndexMap.contains(u)) {
                    const auto idx = static_cast<uint16_t>(unitTable.size());
                    unitIndexMap[u] = idx;
                    unitTable.push_back(u);
                }
            }
        }
    }

    const auto numUnits = static_cast<uint16_t>(unitTable.size());
    BinaryIO::write(out, numUnits);
    for (const auto& unit : unitTable) {
        BinaryIO::write(out, unit);
    }


    for (const auto&[entryName, entryProperties] : m_db) {
        BinaryIO::writeString(out, entryName);
        auto propCount = static_cast<uint32_t>(entryProperties.size());
        BinaryIO::write(out, propCount);

        for (const auto&[propertyName, propertyType, propertyValue] : entryProperties) {
            BinaryIO::writeString(out, propertyName);
            BinaryIO::writeEnum(out, propertyType);

            switch (propertyType) {
                case PropertyType::BOOL:
                    BinaryIO::write(out, static_cast<uint8_t>(std::get<bool>(propertyValue)));
                    break;
                case PropertyType::INT:
                    BinaryIO::write(out, std::get<int64_t>(propertyValue));
                    break;
                case PropertyType::DOUBLE:
                    BinaryIO::write(out, std::get<double>(propertyValue));
                    break;
                case PropertyType::STRING:
                    BinaryIO::writeString(out, std::get<std::string>(propertyValue));
                    break;
                case PropertyType::QUANTITY: {
                    const auto& quantity = std::get<Quantity>(propertyValue);
                    BinaryIO::write(out, quantity.value);
                    BinaryIO::write(out, unitIndexMap[quantity.unit]);
                    break;
                }
                default:
                    throw std::runtime_error("Unknown property type during write");
            }
        }
    }
}

bool BaseDatabase::contains(const std::string& entryName) const noexcept {
    return std::ranges::any_of(m_db, [&](const DatabaseEntry& entry){ return entry.name == entryName; });
}

std::string BaseDatabase::getStringProperty(const std::string& entryName, const std::string& propertyName) const {
    const auto& entry = findEntry(entryName);
    const auto& property  = findProperty(entry, propertyName);
    return std::get<std::string>(property.value);
}

double BaseDatabase::getNumericProperty(const std::string& entryName, const std::string& propertyName) const {
    const auto& entry = findEntry(entryName);
    const auto& property  = findProperty(entry, propertyName);

    return std::visit([]<typename T0>(T0&& val) -> double {
        using T = std::decay_t<T0>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return static_cast<double>(val);
        }
        else if constexpr (std::is_same_v<T, Quantity>) {
            return val.value;
        }
        else {
            throw std::runtime_error("Property is not numeric");
        }
    }, property.value);
}

Quantity BaseDatabase::getQuantityProperty(const std::string& entryName, const std::string& propertyName) const {
    const auto& entry = findEntry(entryName);
    const auto& property  = findProperty(entry, propertyName);

    return std::visit([]<typename T0>(T0&& val) -> Quantity {
        using T = std::decay_t<T0>;
        if constexpr (std::is_same_v<T, Quantity>) {
            return val;
        }
        else {
            throw std::runtime_error("Property is not a Quantity");
        }
    }, property.value);
}

const DatabaseEntry& BaseDatabase::findEntry(const std::string& entryName) const{
    const auto itEntry = std::ranges::find_if(
        m_db, [&](const DatabaseEntry& entry){ return entry.name == entryName; });
    if (itEntry == m_db.end()) {
        throw std::runtime_error(std::format("Unknown entry '{}'", entryName));
    }
    return *itEntry;
}

const DatabaseProperty& BaseDatabase::findProperty(const DatabaseEntry& entry, const std::string& propertyName) {
    const auto itProperty = std::ranges::find_if(
        entry.properties, [&](const DatabaseProperty& property){ return property.name == propertyName; });
    if (itProperty == entry.properties.end()) {
        throw std::runtime_error(std::format("Unknown property '{}' for entry '{}'", propertyName, entry.name));
    }
    return *itProperty;
}
