//
// Physics Simulation Program
// File: atom.h
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Atom particle specialisation encapsulating hyperfine structure
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPES_ATOM_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPES_ATOM_H

#include <cstddef>
#include <vector>

#include "particles/particle.h"

// Atom
//
// Encapsulates hyperfine structure control and spin polarisation for atomic species.
//
// Notes on initialisation:
//   - Constructors mirror Particle while adding atomic spin polarisation and optional hyperfine level tables
//   - If no hyperfine data are supplied, a ground-state level is synthesised using the database nuclear spin
//
// Notes on algorithms:
//   - Hyperfine bookkeeping maintains a vector of HyperfineLevel entries with an active index
//   - selectHyperfineLevel provides both index-based and quantum-number-based lookups with tolerance checks
//
// Notes on output:
//   - printPolarisation() prints the atomic spin vector appended to the Particle::print() output
//
// Supported overloads / operations and functions / methods:
//   - Constructors:           Atom()
//   - Polarisation helpers:   getPolarisation(), setPolarisation(), printPolarisation()
//   - Hyperfine helpers:      setHyperfineLevels(), addHyperfineLevel(), selectHyperfineLevel(),
//                             setHyperfineState(), getHyperfineState(), getNuclearSpin()
class [[nodiscard]] Atom final : public Particle {
    public:
        struct HyperfineLevel {
            int principalQuantumNumber = 0;
            double orbitalAngularMomentum = 0.0;          // L
            double totalElectronicAngularMomentum = 0.0;  // J
            double nuclearSpin = 0.0;                     // I
            double totalAngularMomentum = 0.0;            // F
            double magneticQuantumNumber = 0.0;           // m_F
            bool excited = false;
            Quantity energyShift = Quantity::dimensionless(0.0);
            std::string label;
        };

        Atom(
            std::string type,
            std::string symbol,
            Quantity restMass,
            Quantity charge,
            Quantity spin,
            Quantity time,
            const Vector<3> &position,
            Quantity energy,
            const Vector<3> &momentum,
            const Vector<3> &polarisation,
            std::vector<HyperfineLevel> hyperfineLevels = {},
            std::size_t activeLevelIndex = 0
        );

        explicit Atom(
            std::string type,
            const Quantity& time = Quantity(),
            const Vector<3>& position = Vector<3>(),
            const Quantity& energy = Quantity(),
            const Vector<3>& momentum = Vector<3>(),
            const Vector<3>& polarisation = Vector<3>(),
            std::vector<HyperfineLevel> hyperfineLevels = {},
            std::size_t activeLevelIndex = 0
        );

        [[nodiscard]] const Vector<3>& getPolarisation() const { return this->m_polarisation; }
        void setPolarisation(const Vector<3>& polarisation) { this->m_polarisation = polarisation; }

        void setHyperfineLevels(std::vector<HyperfineLevel> levels, std::size_t activeIndex = 0);
        void addHyperfineLevel(const HyperfineLevel& level);
        bool selectHyperfineLevel(std::size_t index);
        bool selectHyperfineLevel(double F, double mF, bool excited);
        void setHyperfineState(const HyperfineLevel& level);
        [[nodiscard]] const HyperfineLevel& getHyperfineState() const;
        [[nodiscard]] double getNuclearSpin() const noexcept { return this->m_nuclearSpin; }

    protected:
        void printPolarisation(std::ostream& stream) const override;

    private:
        Vector<3> m_polarisation;
        std::vector<HyperfineLevel> m_hyperfineLevels;
        std::size_t m_activeHyperfineIndex = 0;
        double m_nuclearSpin = 0.0;

        static HyperfineLevel normalizeHyperfineLevel(const HyperfineLevel& level, double defaultNuclearSpin);
        void ensureHyperfineState();
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPES_ATOM_H