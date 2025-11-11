//
// Physics Simulation Program
// File: object_manager.h
// Created by Tobias Sharman on 11/11/2025
//
// Description:
//   - Manage lifetime of world objects and expose helpers for accessing the active world
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_OBJECT_MANAGER_H
#define PHYSICS_SIMULATION_PROGRAM_OBJECT_MANAGER_H

#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>

#include "objects/object.h"

// ObjectManager
//
// Manage objects by managing the world (highest level object of the object tree). Allows for definition of multiple
// worlds in the program that can be switched between. It is recommended to use the already defined object_manager at
// the bottom of this file lest issues be caused through usage of the standard manager throughout the simulation scripts
//
// Notes on initialisation:
//   - Worlds must be created through createWorld<T>() so ownership stays inside the manager
//   - The first world created automatically becomes the active world; subsequent ones require setActiveWorld()
//   - Stored worlds are never deleted by callers; pointers returned by the manager remain valid for the lifetime of the
//     program
//
// Notes on algorithms:
//   - getActiveWorld() is context-aware: it throws if no world is active, using the provided context string to describe
//     the failing operation
//   - objectBelongsToWorld() walks parent pointers until reaching the root, mirroring the logic used in Object
//
// Supported overloads / operations and functions / methods:
//   - Construction:           createWorld<T>()
//   - Getters:                getActiveWorld(), getActiveWorldAt(), getWorldCount()
//   - Setters:                setActiveWorld()
//   - Relationship checks:    objectBelongsToWorld(), objectBelongsToActiveWorld()
//
// Example usage:
//   auto* world = g_objectManager.createWorld<Box>(name("World"), material("vacuum"), size(...));
//   g_objectManager.setActiveWorld(world);
//   const auto* active = g_objectManager.getActiveWorld("initialise detectors");
class ObjectManager {
    public:
        template<typename T, typename... Args>
        requires (!(std::same_as<std::decay_t<Args>, ParentTag> || ...))
        T* createWorld(Args&&... args) {
            static_assert(std::derived_from<T, Object>, "World type must derive from Object");

            auto world = std::make_unique<T>(std::forward<Args>(args)...);
            T* pointer = world.get();
            m_worlds.push_back(std::move(world));

            if (!m_activeWorldIndex.has_value()) {
                m_activeWorldIndex = m_worlds.size() - 1;
            }

            return pointer;
        }

        // Getters
        [[nodiscard]] Object* getActiveWorld(std::string_view context = {});
        [[nodiscard]] const Object* getActiveWorld(std::string_view context = {}) const;
        [[nodiscard]] Object* getWorldAt(std::size_t index);
        [[nodiscard]] const Object* getWorldAt(std::size_t index) const;
        [[nodiscard]] std::size_t getWorldCount() const noexcept { return m_worlds.size(); }

        // Setters
        void setActiveWorld(std::size_t index);
        void setActiveWorld(const Object* world);

        // Object belongs to world check method
        [[nodiscard]] static bool objectBelongsToWorld(const Object* object, const Object* world) noexcept;

        // Object belongs to the current active world check method
        [[nodiscard]] bool objectBelongsToActiveWorld(const Object* object) const;

    private:
        // Find root method
        //
        // Locates the highest level parent of an object
        static const Object* findRoot(const Object* object) noexcept;

        std::vector<std::unique_ptr<Object>> m_worlds;
        std::optional<std::size_t> m_activeWorldIndex;
    };

// Global instance used to manage objects
extern ObjectManager g_objectManager;

#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_MANAGER_H