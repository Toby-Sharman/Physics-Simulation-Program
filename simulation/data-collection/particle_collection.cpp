//
// Physics Simulation Program
// File: particle_collection.cpp
// Created by Tobias Sharman on 15/09/2025
//
// Description:
//   - Implementation of particle_collection.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "particle_collection.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace {
    // Per-detector logging state so we only create folders/files once per run
    struct DetectorLogContext {
        std::filesystem::path baseFolder;                                        // Detector-specific output root
        std::string baseFilename;                                                // Prefix used when creating new CSV files
        std::unordered_map<std::string, std::unique_ptr<std::ofstream>> streams; // Cache of open streams keyed by particle type
        std::mutex mutex;                                                        // Guards stream map and file writes
    };

    std::shared_ptr<DetectorLogContext> getContext(
        const Object *detector,
        const std::string_view baseFolder,
        const std::string_view baseFilename
    ) {
        static std::mutex contextMapMutex;
        static std::unordered_map<const Object *, std::shared_ptr<DetectorLogContext>> contextMap;

        std::scoped_lock mapLock(contextMapMutex);
        auto it = contextMap.find(detector);
        if (it == contextMap.end()) {
            auto context = std::make_shared<DetectorLogContext>();
            context->baseFolder = std::filesystem::path(baseFolder);
            context->baseFilename = std::string(baseFilename);
            std::error_code ec;
            std::filesystem::create_directories(context->baseFolder, ec);
            if (ec) {
                std::cerr << std::format(
                    "Failed to create detector base folder '{}' : {}\n",
                    context->baseFolder.string(),
                    ec.message()
                );
            }
            it = contextMap.emplace(detector, std::move(context)).first;
        }
        return it->second;
    }

    std::string sanitiseComponent(const std::string_view component) {
        std::string result;
        result.reserve(component.size());
        for (const unsigned char ch : component) {
            if (std::isalnum(ch) || ch == '_' || ch == '-') {
                result.push_back(static_cast<char>(ch));
            } else {
                result.push_back('_');
            }
        }
        if (result.empty()) {
            result = "undefined";
        }
        return result;
    }

    std::optional<std::filesystem::path> reserveLogFile(
        const std::filesystem::path &folder,
        const std::string &baseFilename
    ) {
        constexpr int maxFiles = std::numeric_limits<int>::max(); // If there is this many existing files already likely there is a problem to review
        for (int counter = 1; counter < maxFiles; ++counter) {
            if (auto candidate = folder / std::format("{}{}{}", baseFilename, counter, ".csv");
                !std::filesystem::exists(candidate)) {
                return candidate;
            }
        }
        return std::nullopt;
    }

    // Retrieve (or lazily create) the CSV stream associated with a particle type
    // Callers must hold context.mutex before invoking this helper
    std::ofstream *getStreamLocked(DetectorLogContext &context, const std::string &type) {
        if (const auto existing = context.streams.find(type); existing != context.streams.end()) {
            return existing->second.get();
        }

        const auto folder = context.baseFolder / sanitiseComponent(type);
        std::error_code ec;
        std::filesystem::create_directories(folder, ec);
        if (ec) {
            std::cerr << std::format("Failed to create detector folder '{}': {}\n", folder.string(), ec.message());
            return nullptr;
        }

        const auto filename = reserveLogFile(folder, context.baseFilename);
        if (!filename) {
            std::cerr << std::format("Failed to reserve detector log filename inside '{}'\n", folder.string());
            return nullptr;
        }

        auto file = std::make_unique<std::ofstream>(*filename, std::ios::app);
        if (!file->is_open()) {
            std::cerr << std::format("Failed to open detector log file: {}\n", filename->string());
            return nullptr;
        }

        auto [entryIt, inserted] = context.streams.emplace(type, std::move(file));
        (void)inserted;
        return entryIt->second.get();
    }
} // namespace

void logEnergyIfInside(
    std::unique_ptr<Particle> &particle,
    const Object *detector,
    const std::string_view &baseFolder,
    const std::string_view &baseFilename
) {
    if (!particle || detector == nullptr) { return; }

    if (!detector->contains(particle->getPosition())) { return; }

    auto context = getContext(detector, baseFolder, baseFilename);
    if (!context) { return; }

    std::scoped_lock lock(context->mutex);
    if (auto *stream = getStreamLocked(*context, particle->getType())) {
        *stream << particle->getEnergy() << "\n";
        particle.reset();
    }
}
