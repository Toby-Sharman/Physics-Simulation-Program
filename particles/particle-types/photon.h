//
// Physics Simulation Program
// File: photon.h
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Photon particle specialisation
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPES_PHOTON_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPES_PHOTON_H

#include "particles/particle.h"

// Photon
//
// Notes on initialisation:
//   - Mirrors Particle constructors while adding an explicit Stoke's vector polarisation argument
//   - Default constructor path pulls particle properties from the particle database
//
// Notes on algorithms:
//   - Polarisation access is purely accessor/mutator; propagation is handled elsewhere
//
// Notes on output:
//   - printPolarisation() prints (I, Q, U, V) following the base Particle::print() stream
//
// Supported overloads / operations and functions / methods:
//   - Constructors:            Photon()
//   - Polarisation helpers:    getPolarisation(), setPolarisation(), printPolarisation()
class [[nodiscard]] Photon final : public Particle {
    public:
        Photon(
            std::string type,
            std::string symbol,
            Quantity restMass,
            Quantity charge,
            Quantity spin,
            Quantity time,
            const Vector<3> &position,
            Quantity energy,
            const Vector<3> &momentum,
            const Vector<4> &polarisation
        );

        explicit Photon(
            std::string type,
            const Quantity& time = Quantity(),
            const Vector<3>& position = Vector<3>(),
            const Quantity& energy = Quantity(),
            const Vector<3>& momentum = Vector<3>(),
            const Vector<4>& polarisation = Vector<4>()
        );

        [[nodiscard]] const Vector<4>& getPolarisation() const { return this->m_polarisation; }
        void setPolarisation(const Vector<4>& polarisation) { this->m_polarisation = polarisation; }

    protected:
        void printPolarisation(std::ostream& stream) const override;

    private:
        Vector<4> m_polarisation;
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLE_TYPES_PHOTON_H