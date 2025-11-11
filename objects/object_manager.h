//
// Physics Simulation Program
// File: object_manager.h
// Created by Tobias Sharman on 11/11/2025
//
// Description:
//   - Describe an object manager to manage different worlds
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
#include <type_traits>
#include <vector>

#include "objects/object.h"

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

        void setActiveWorld(std::size_t index);
        void setActiveWorld(const Object* world);

        [[nodiscard]] Object* getActiveWorld() noexcept;
        [[nodiscard]] const Object* getActiveWorld() const noexcept;

        [[nodiscard]] std::size_t getWorldCount() const noexcept { return m_worlds.size(); }

        [[nodiscard]] Object* getWorldAt(std::size_t index);
        [[nodiscard]] const Object* getWorldAt(std::size_t index) const;

        [[nodiscard]] static bool objectBelongsToWorld(const Object* object, const Object* world) noexcept;
        [[nodiscard]] bool objectBelongsToActiveWorld(const Object* object) const noexcept;

    private:
        static const Object* findRoot(const Object* object) noexcept;

        std::vector<std::unique_ptr<Object>> m_worlds;
        std::optional<std::size_t> m_activeWorldIndex;
};

extern ObjectManager g_objectManager;

#endif //PHYSICS_SIMULATION_PROGRAM_OBJECT_MANAGER_H