//
// Physics Simulation Program
// File: object_manager.cpp
// Created by Tobias Sharman on 11/11/2025
//
// Description:
//   - Implementation of object_manager.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "objects/object_manager.h"

#include <algorithm>
#include <format>

void ObjectManager::setActiveWorld(std::size_t index) {
    if (index >= m_worlds.size()) {
        throw std::out_of_range(std::format(
            "Cannot set active world to index {} because there are only {} worlds",
            index,
            m_worlds.size()
        ));
    }

    m_activeWorldIndex = index;
}

void ObjectManager::setActiveWorld(const Object* world) {
    if (world == nullptr) {
        throw std::invalid_argument("Cannot set active world because provided pointer is null");
    }

    for (std::size_t index = 0; index < m_worlds.size(); ++index) {
        if (m_worlds[index].get() == world) {
            m_activeWorldIndex = index;
            return;
        }
    }

    throw std::invalid_argument("Cannot set active world because provided pointer is not managed");
}

Object* ObjectManager::getActiveWorld() noexcept {
    if (!m_activeWorldIndex.has_value() || m_worlds.empty()) {
        return nullptr;
    }

    const auto index = std::min(m_activeWorldIndex.value(), m_worlds.size() - 1);
    return m_worlds[index].get();
}

const Object* ObjectManager::getActiveWorld() const noexcept {
    return const_cast<ObjectManager*>(this)->getActiveWorld();
}

Object* ObjectManager::getWorldAt(std::size_t index) {
    if (index >= m_worlds.size()) {
        throw std::out_of_range(std::format(
            "World index {} is outside the range [0, {})",
            index,
            m_worlds.size()
        ));
    }

    return m_worlds[index].get();
}

const Object* ObjectManager::getWorldAt(std::size_t index) const {
    return const_cast<ObjectManager*>(this)->getWorldAt(index);
}

bool ObjectManager::objectBelongsToWorld(const Object* object, const Object* world) noexcept {
    if (!object || !world) {
        return false;
    }

    return findRoot(object) == world;
}

bool ObjectManager::objectBelongsToActiveWorld(const Object* object) const noexcept {
    const auto* world = getActiveWorld();
    if (!world) {
        return false;
    }

    return objectBelongsToWorld(object, world);
}

const Object* ObjectManager::findRoot(const Object* object) noexcept {
    const Object* current = object;
    while (current != nullptr && current->getParent() != nullptr) {
        current = current->getParent();
    }

    return current;
}

ObjectManager g_objectManager;