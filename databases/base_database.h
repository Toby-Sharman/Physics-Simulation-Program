//
// Created by Tobias Sharman on 13/09/2025.
//

#ifndef BASE_DATABASE_H
#define BASE_DATABASE_H

#include "quantity.h"

#include "binary_file_IO.h"  // Include BinaryIO namespace

#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <format>
#include <algorithm>

// Generic property
struct DBProperty {
    std::string name;
    PropertyType type = PropertyType::DOUBLE;
    std::variant<int32_t, double, bool, std::string, Quantity> value;
};

// Generic entry
struct DBEntry {
    std::string name;
    std::vector<DBProperty> properties;
};

// BaseDB class
template <typename Derived>
class BaseDB {
public:

    // Load from binary file
    static void loadFromBinary(const std::string& filename) {
        db_.clear();
        std::ifstream in(filename, std::ios::binary);
        if (!in)
            throw std::runtime_error(std::format("Cannot open database file '{}'", filename));

        // Step 1: read unit table
        uint16_t numUnits;
        if (!BinaryIO::read(in, numUnits))
            throw std::runtime_error("Failed to read number of units");
        std::vector<std::string> unitTable(numUnits);
        for (uint16_t i = 0; i < numUnits; ++i)
            if (!BinaryIO::readString(in, unitTable[i]))
                throw std::runtime_error("Failed to read unit string");

        // Step 2: read entries
        while (true) {
            DBEntry entry;
            if (!BinaryIO::readString(in, entry.name)) break; // EOF

            uint32_t propCount;
            if (!BinaryIO::read(in, propCount))
                throw std::runtime_error(std::format("Unexpected EOF reading property count for '{}'", entry.name));

            entry.properties.reserve(propCount);

            for (uint32_t i = 0; i < propCount; ++i) {
                DBProperty prop;
                if (!BinaryIO::readString(in, prop.name))
                    throw std::runtime_error(std::format("Unexpected EOF reading property name for '{}'", entry.name));
                if (!BinaryIO::readEnum(in, prop.type))
                    throw std::runtime_error(std::format("Unexpected EOF reading property type for '{}.{}'", entry.name, prop.name));

                switch (prop.type) {
                    case PropertyType::BOOL: {
                        uint8_t b;
                        if (!BinaryIO::read(in, b))
                            throw std::runtime_error(std::format("Unexpected EOF reading bool '{}.{}'", entry.name, prop.name));
                        prop.value = (b != 0);
                        break;
                    }
                    case PropertyType::INT: {
                        int32_t v;
                        if (!BinaryIO::read(in, v))
                            throw std::runtime_error(std::format("Unexpected EOF reading int '{}.{}'", entry.name, prop.name));
                        prop.value = v;
                        break;
                    }
                    case PropertyType::DOUBLE: {
                        double v;
                        if (!BinaryIO::read(in, v))
                            throw std::runtime_error(std::format("Unexpected EOF reading double '{}.{}'", entry.name, prop.name));
                        prop.value = v;
                        break;
                    }
                    case PropertyType::STRING: {
                        std::string s;
                        if (!BinaryIO::readString(in, s))
                            throw std::runtime_error(std::format("Unexpected EOF reading string '{}.{}'", entry.name, prop.name));
                        prop.value = s;
                        break;
                    }
                    case PropertyType::QUANTITY: {
                        Quantity q;
                        if (!BinaryIO::readQuantity(in, q, unitTable))
                            throw std::runtime_error(std::format("Unexpected EOF reading Quantity '{}.{}'", entry.name, prop.name));
                        prop.value = q;
                        break;
                    }
                    default:
                        throw std::runtime_error(std::format("Unknown property type for '{}.{}'", entry.name, prop.name));
                }

                entry.properties.push_back(std::move(prop));
            }

            db_.push_back(std::move(entry));
        }
    }

    // Write DB to binary file
    static void saveToBinary(const std::string& filename) {
        std::ofstream out(filename, std::ios::binary);
        if (!out) throw std::runtime_error(std::format("Cannot open file '{}'", filename));

        // Step 1: collect unique units
        std::unordered_map<std::string, uint16_t> unitIndexMap;
        std::vector<std::string> unitTable;

        for (const auto&[name, properties] : db_) {
            for (const auto& prop : properties) {
                if (prop.type == PropertyType::QUANTITY) {
                    if (const std::string& u = std::get<Quantity>(prop.value).unit; !unitIndexMap.contains(u)) {
                        const auto idx = static_cast<uint16_t>(unitTable.size());
                        unitTable.push_back(u);
                        unitIndexMap[u] = idx;
                    }
                }
            }
        }

        // Step 2: write unit table
        const auto numUnits = static_cast<uint16_t>(unitTable.size());
        BinaryIO::write(out, numUnits);
        for (const auto& u : unitTable)
            BinaryIO::writeString(out, u);

        // Step 3: write entries
        for (const auto&[dbName, properties] : db_) {
            BinaryIO::writeString(out, dbName);
            auto propCount = static_cast<uint32_t>(properties.size());
            BinaryIO::write(out, propCount);

            for (const auto&[propName, type, value] : properties) {
                BinaryIO::writeString(out, propName);
                BinaryIO::writeEnum(out, type);

                switch (type) {
                    case PropertyType::BOOL:
                        BinaryIO::write(out, static_cast<uint8_t>(std::get<bool>(value)));
                        break;
                    case PropertyType::INT:
                        BinaryIO::write(out, std::get<int32_t>(value));
                        break;
                    case PropertyType::DOUBLE:
                        BinaryIO::write(out, std::get<double>(value));
                        break;
                    case PropertyType::STRING:
                        BinaryIO::writeString(out, std::get<std::string>(value));
                        break;
                    case PropertyType::QUANTITY:
                        BinaryIO::writeQuantity(out, std::get<Quantity>(value), unitIndexMap);
                        break;
                    default:
                        throw std::runtime_error("Unknown property type during write");
                }
            }
        }
    }

    // Check the db contains a specified entry
    static bool contains(const std::string& entryName) {
        return std::ranges::any_of(db_, [&](const DBEntry& e){ return e.name == entryName; });
    }

    // Access numeric property
    static double getProperty(const std::string& entryName, const std::string& property) {
        auto itEntry = std::find_if(db_.begin(), db_.end(),
            [&](const DBEntry& e){ return e.name == entryName; });
        if (itEntry == db_.end())
            throw std::runtime_error(std::format("Unknown entry '{}'", entryName));

        auto itProp = std::find_if(itEntry->properties.begin(), itEntry->properties.end(),
            [&](const DBProperty& p){ return p.name == property; });
        if (itProp == itEntry->properties.end())
            throw std::runtime_error(std::format("Unknown property '{}' for entry '{}'", property, entryName));

        return std::visit([]<typename T0>(T0&& val) -> double {
            using T = std::decay_t<T0>;
            if constexpr(std::is_same_v<T, int32_t> || std::is_same_v<T, float> || std::is_same_v<T, double>)
                return static_cast<double>(val);
            else if constexpr(std::is_same_v<T, Quantity>)
                return val.value;
            else
                throw std::runtime_error("Property is not numeric");
        }, itProp->value);
    }

    // Access Quantity property
    static Quantity getQuantity(const std::string& entryName, const std::string& property) {
        auto itEntry = std::find_if(db_.begin(), db_.end(),
            [&](const DBEntry& e){ return e.name == entryName; });
        if (itEntry == db_.end())
            throw std::runtime_error(std::format("Unknown entry '{}'", entryName));

        auto itProp = std::find_if(itEntry->properties.begin(), itEntry->properties.end(),
            [&](const DBProperty& p){ return p.name == property; });
        if (itProp == itEntry->properties.end())
            throw std::runtime_error(std::format("Unknown property '{}' for entry '{}'", property, entryName));

        return std::visit([]<typename T0>(T0&& val) -> Quantity {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, Quantity>)
                return val;
            else
                throw std::runtime_error("Property is not a Quantity");
        }, itProp->value);
    }

protected:
    static inline std::vector<DBEntry> db_;
};


#endif //BASE_DATABASE_H
