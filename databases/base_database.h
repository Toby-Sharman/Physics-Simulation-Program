//
// Created by Tobias Sharman on 13/09/2025.
//

#ifndef SIMULATION_PROGRAM_BASE_DATABASE_H
#define SIMULATION_PROGRAM_BASE_DATABASE_H

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
    std::variant<int32_t, double, bool, std::string> value;
};

// Generic entry
struct DBEntry {
    std::string name;
    std::vector<DBProperty> properties;
};

// Base class (CRTP pattern for static polymorphism)
template <typename Derived>
class BaseDB {
public:

    // Load database from binary file
    static void loadFromBinary(const std::string& filename) {
        db_.clear();
        std::ifstream in(filename, std::ios::binary);
        if (!in)
            throw std::runtime_error(std::format("Cannot open database file '{}'", filename));

        while (true) {
            DBEntry entry;
            if (!BinaryIO::readString(in, entry.name)) break;  // EOF reached

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
                    default:
                        throw std::runtime_error(std::format("Unknown property type for '{}.{}'", entry.name, prop.name));
                }

                entry.properties.push_back(std::move(prop));
            }

            db_.push_back(std::move(entry));
        }
    }

    // Get a single numeric property (throws if type mismatch or not found)
    static double getProperty(const std::string& entryName, const std::string& property) {
        auto itEntry = std::find_if(db_.begin(), db_.end(),
            [&](const DBEntry& e){ return e.name == entryName; });

        if (itEntry == db_.end())
            throw std::runtime_error(std::format("Unknown entry '{}'", entryName));

        auto itProp = std::find_if(itEntry->properties.begin(), itEntry->properties.end(),
            [&](const DBProperty& p){ return p.name == property; });

        if (itProp == itEntry->properties.end())
            throw std::runtime_error(std::format("Unknown property '{}' for entry '{}'", property, entryName));

        // Only allow numeric types
        return std::visit([]<typename T0>(T0&& val) -> double {
            using T = std::decay_t<T0>;
            if constexpr(std::is_same_v<T, int32_t> || std::is_same_v<T, float> || std::is_same_v<T, double>)
                return static_cast<double>(val);
            else
                throw std::runtime_error("Property is not numeric");
        }, itProp->value);
    }

    // Get all properties as name-value pairs (numeric only)
    static std::vector<std::pair<std::string, double>> getProperties(const std::string& entryName) {
        auto itEntry = std::find_if(db_.begin(), db_.end(),
            [&](const DBEntry& e){ return e.name == entryName; });

        if (itEntry == db_.end())
            throw std::runtime_error(std::format("Unknown entry '{}'", entryName));

        std::vector<std::pair<std::string, double>> result;
        for (const auto& prop : itEntry->properties) {
            if (prop.type == PropertyType::BOOL || prop.type == PropertyType::STRING)
                continue;  // skip non-numeric

            result.emplace_back(prop.name, std::visit([](auto&& val) -> double {
                return static_cast<double>(val);
            }, prop.value));
        }
        return result;
    }

    // Check if an entry exists
    static bool contains(const std::string& entryName) {
        return std::ranges::any_of(db_, [&](const DBEntry& e){ return e.name == entryName; });
    }

//protected:
    static inline std::vector<DBEntry> db_;
};

#endif //SIMULATION_PROGRAM_BASE_DATABASE_H
