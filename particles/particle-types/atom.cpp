//
// Physics Simulation Program
// File: atom.cpp
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Implementation of atom.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "particles/particle-types/atom.h"

#include <cmath>  // For std::abs ignore warning
#include <utility>

#include "core/globals.h"
#include "databases/particle-data/particle_database.h"

namespace {
Atom::HyperfineLevel makeDefaultHyperfineLevel(const double nuclearSpin) {
    Atom::HyperfineLevel level{};
    level.nuclearSpin = nuclearSpin;
    level.label = "Ground";
    return level;
}
} // namespace

Atom::Atom(
    std::string type,
    std::string symbol,
    const Quantity restMass,
    const Quantity charge,
    const Quantity spin,
    const Quantity time,
    const Vector<3> &position,
    const Quantity energy,
    const Vector<3> &momentum,
    const Vector<3> &polarisation,
    std::vector<HyperfineLevel> hyperfineLevels,
    const std::size_t activeLevelIndex)
    : Particle(std::move(type), std::move(symbol), restMass, charge, spin, time, position, energy, momentum)
{
    this->setPolarisation(polarisation);
    if (const auto nuclearSpin = g_particleDatabase.getNuclearSpin(getType())) {
        this->m_nuclearSpin = *nuclearSpin;
    } else if (!hyperfineLevels.empty() && hyperfineLevels.front().nuclearSpin != 0.0) {
        this->m_nuclearSpin = hyperfineLevels.front().nuclearSpin;
    }
    this->setHyperfineLevels(std::move(hyperfineLevels), activeLevelIndex);
}

Atom::Atom(
    std::string type,
    const Quantity& time,
    const Vector<3>& position,
    const Quantity& energy,
    const Vector<3>& momentum,
    const Vector<3>& polarisation,
    std::vector<HyperfineLevel> hyperfineLevels,
    const std::size_t activeLevelIndex)
    : Particle(std::move(type), time, position, energy, momentum)
{
    this->setPolarisation(polarisation);
    if (const auto nuclearSpin = g_particleDatabase.getNuclearSpin(getType())) {
        this->m_nuclearSpin = *nuclearSpin;
    } else if (!hyperfineLevels.empty() && hyperfineLevels.front().nuclearSpin != 0.0) {
        this->m_nuclearSpin = hyperfineLevels.front().nuclearSpin;
    }
    this->setHyperfineLevels(std::move(hyperfineLevels), activeLevelIndex);
}

void Atom::setHyperfineLevels(std::vector<HyperfineLevel> levels, const std::size_t activeIndex) {
    if (levels.empty()) {
        levels.push_back(makeDefaultHyperfineLevel(this->m_nuclearSpin));
    } else {
        for (auto& level : levels) {
            level = normalizeHyperfineLevel(level, this->m_nuclearSpin);
        }
    }
    this->m_hyperfineLevels = std::move(levels);
    this->m_activeHyperfineIndex = (activeIndex < this->m_hyperfineLevels.size()) ? activeIndex : 0;
}

void Atom::addHyperfineLevel(const HyperfineLevel& level) {
    this->m_hyperfineLevels.push_back(normalizeHyperfineLevel(level, this->m_nuclearSpin));
}

bool Atom::selectHyperfineLevel(const std::size_t index) {
    if (this->m_hyperfineLevels.empty()) {
        return false;
    }
    if (index >= this->m_hyperfineLevels.size()) {
        return false;
    }
    this->m_activeHyperfineIndex = index;
    return true;
}

bool Atom::selectHyperfineLevel(const double F, const double mF, const bool excited) {
    if (this->m_hyperfineLevels.empty()) {
        return false;
    }
    constexpr double tolerance = Globals::Constant::Program::hyperfineSelectionTolerance;
    for (std::size_t i = 0; i < this->m_hyperfineLevels.size(); ++i) {
        if (const auto& level = this->m_hyperfineLevels[i];
            std::abs(level.totalAngularMomentum - F) < tolerance &&
            std::abs(level.magneticQuantumNumber - mF) < tolerance &&
            level.excited == excited)
        {
            this->m_activeHyperfineIndex = i;
            return true;
        }
    }
    return false;
}

void Atom::setHyperfineState(const HyperfineLevel& level) {
    ensureHyperfineState();
    this->m_hyperfineLevels[this->m_activeHyperfineIndex] = normalizeHyperfineLevel(level, this->m_nuclearSpin);
}

const Atom::HyperfineLevel& Atom::getHyperfineState() const {
    if (this->m_hyperfineLevels.empty()) {
        static const HyperfineLevel defaultLevel{};
        return defaultLevel;
    }
    return this->m_hyperfineLevels[this->m_activeHyperfineIndex];
}

void Atom::printPolarisation(std::ostream& stream) const {
    stream << "Polarisation (atomic spin): ";
    this->m_polarisation.print();
}

Atom::HyperfineLevel Atom::normalizeHyperfineLevel(const HyperfineLevel& level, const double defaultNuclearSpin) {
    HyperfineLevel normalized = level;
    if (normalized.nuclearSpin == 0.0) {
        normalized.nuclearSpin = defaultNuclearSpin;
    }
    return normalized;
}

void Atom::ensureHyperfineState() {
    if (this->m_hyperfineLevels.empty()) {
        this->m_hyperfineLevels.push_back(makeDefaultHyperfineLevel(this->m_nuclearSpin));
        this->m_activeHyperfineIndex = 0;
    }
}