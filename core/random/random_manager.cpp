//
// Physics Simulation Program
// File: random_manager.cpp
// Created by Tobias Sharman on 02/11/2025
//
// Description:
//   - Implementation of random_manager.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "core/random/random_manager.h"

#include <atomic>
#include <limits>
#include <mutex>
#include <optional>
#include <unordered_map>

namespace random_manager {
    namespace {
        // Constants used to derive deterministic seeds across the application
        constexpr std::uint64_t k_defaultMasterSeed = 0x2006200420062004ULL;
        constexpr std::uint64_t k_goldenRatio64     = 0x9E3779B97F4A7C15ull;
        constexpr std::uint64_t k_streamSalt        = 0x0211202502112025ULL;

        // SplitMix64 mixer gives strong avalanche behaviour for 64-bit values with a uniform spread
        constexpr std::uint64_t mix64(std::uint64_t value) noexcept {
            value ^= value >> 33;
            value *= 0xff51afd7ed558ccdULL;
            value ^= value >> 33;
            value *= 0xc4ceb9fe1a85ec53ULL;
            value ^= value >> 33;
            return value;
        }

        struct StreamKey {
            Stream stream;
            std::size_t index; // For substreams, i.e. thread, or particle by an id

            bool operator==(const StreamKey& other) const noexcept {
                return this->stream == other.stream && this->index == other.index;
            }
        };

        struct StreamKeyHasher {
            std::size_t operator()(const StreamKey& key) const noexcept {
                const auto stream = static_cast<std::uint64_t>(key.stream);
                return key.index ^ mix64(stream + k_goldenRatio64);
            }
        };

        struct EngineWrapper { // Ignore warning the engine is properly and immediately reseeded appropriately
            std::ranlux48 engine;
            std::uint64_t seedUsed = std::numeric_limits<std::uint64_t>::max();
        };

        std::atomic<std::uint64_t> g_masterSeedAtomic{0};
        std::mutex g_seedMutex;
        std::unordered_map<Stream, std::uint64_t> g_streamSeeds;
        std::atomic<std::uint64_t> g_seedVersion{0};

        thread_local std::unordered_map<StreamKey, EngineWrapper, StreamKeyHasher> g_threadEngines;
        thread_local std::uint64_t g_cachedSeedVersion = std::numeric_limits<std::uint64_t>::max();

        std::uint64_t ensureMasterSeed() {
            if (const auto current = g_masterSeedAtomic.load(std::memory_order_acquire); current != 0) {
                return current;
            }

            std::scoped_lock lock(g_seedMutex);
            auto current = g_masterSeedAtomic.load(std::memory_order_relaxed);
            if (current == 0) {
                std::random_device rd;
                current = static_cast<std::uint64_t>(rd());
                if (current == 0) {
                    current = k_defaultMasterSeed;
                }
                g_masterSeedAtomic.store(current, std::memory_order_release);
                g_seedVersion.fetch_add(1, std::memory_order_relaxed);
            }
            return current;
        }

        [[nodiscard]] constexpr std::uint64_t sanitizedStreamSeed(Stream stream, const std::uint64_t seed) noexcept {
            if (seed != 0) {
                return seed;
            }
            const auto salt = static_cast<std::uint64_t>(stream) ^ k_streamSalt;
            return mix64(salt);
        }

        std::uint64_t resolvedStreamSeed(Stream stream) {
            if (stream == Stream::Master) {
                return ensureMasterSeed();
            }

            const auto override = [&]() -> std::optional<std::uint64_t> {
                std::scoped_lock lock(g_seedMutex);
                if (const auto it = g_streamSeeds.find(stream); it != g_streamSeeds.end()) {
                    return it->second;
                }
                return std::nullopt;
            }();

            if (override.has_value()) {
                return *override;
            }

            const auto master = ensureMasterSeed();
            const auto offset = mix64(static_cast<std::uint64_t>(stream) + k_goldenRatio64);
            return mix64(master ^ offset);
        }

        std::uint64_t finalSeed(const Stream stream, const std::size_t index) {
            const auto base = resolvedStreamSeed(stream);
            const auto composed = mix64(base + mix64(index + k_goldenRatio64));
            return composed;
        }

        void reseedThreadEnginesIfNeeded() {
            const auto globalVersion = g_seedVersion.load(std::memory_order_acquire);
            if (g_cachedSeedVersion == globalVersion || g_threadEngines.empty()) {
                return;
            }

            for (auto&[key, wrapper] : g_threadEngines) {
                const auto seed = finalSeed(key.stream, key.index);
                wrapper.engine.seed(seed);
                wrapper.seedUsed = seed;
            }

            g_cachedSeedVersion = globalVersion;
        }

    } // namespace

    std::uint64_t getMasterSeed() {
        return ensureMasterSeed();
    }

    std::uint64_t getStreamSeed(const Stream stream) {
        return resolvedStreamSeed(stream);
    }

    void setMasterSeed(std::uint64_t seed) noexcept {
        if (seed == 0) {
            seed = k_defaultMasterSeed;
        }

        {
            std::scoped_lock lock(g_seedMutex);
            g_masterSeedAtomic.store(seed, std::memory_order_release);
        }
        g_seedVersion.fetch_add(1, std::memory_order_release);
    }

    void setStreamSeed(const Stream stream, const std::uint64_t seed) noexcept {
        if (stream == Stream::Master) {
            setMasterSeed(seed);
            return;
        }

        {
            std::scoped_lock lock(g_seedMutex);
            g_streamSeeds[stream] = sanitizedStreamSeed(stream, seed);
        }
        g_seedVersion.fetch_add(1, std::memory_order_release);
    }

    std::ranlux48& engine(const Stream stream, const std::size_t streamIndex) {
        ensureMasterSeed();
        reseedThreadEnginesIfNeeded();

        const StreamKey key{stream, streamIndex};
        auto [iterator, inserted] = g_threadEngines.try_emplace(key);
        auto&[engine, seedUsed] = iterator->second;
        if (const auto seed = finalSeed(stream, streamIndex); inserted || seedUsed != seed) {
            engine.seed(seed);
            seedUsed = seed;
        }
        return engine;
    }

    void resetCachedEngines() noexcept {
        g_threadEngines.clear();
        g_cachedSeedVersion = std::numeric_limits<std::uint64_t>::max();
    }
}
