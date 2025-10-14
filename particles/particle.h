//
// Physics Simulation Program
// File: particle.h
// Created by Tobias Sharman on 28/08/2025
//
// Description:
//   - Describe particle class and a manager to hold all the particles
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLES_H

#include <cmath> // For std::abs ignore warning
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/maths/vector.h"
#include "core/maths/utilities/quantity.h"

// Particle
//
// Notes on initialisation:
//   - Can be constructed via specified arguments for all particle fields or via a particle type and its position,
//     momentum, and polarisation.
//         -> If constructing via the particle type it must be described in the particle database
//
// Notes on algorithms:
//   - Lorentz factor is not used to update particle position
//   - Massless check checks if the mass is below the tolerance described in globals.h
//
// Notes on output:
//
// Supported overloads / operations and functions / methods:
//   - Constructor:            Particle()
//   - Getters:                get_____() (type, symbol, restMass, charge, spin, position, momentum, polarisation)
//   - Setters:                set_____() (type, symbol, restMass, charge, spin, position, momentum, polarisation)
//   - Lorentz factor:         gamma()
//   - Massless check:         isMassless()
//   - Printing:               print()
//
// Example usage:
class [[nodiscard]] Particle {
    public:
        // Constructor for custom particles
        Particle(
            std::string type,
            std::string symbol,
            Quantity restMass,
            Quantity charge,
            Quantity spin,
            Vector<4> position,    // (ct, x, y, z)
            Vector<4> momentum,    // (E/c, p_x, p_y, p_z)
            Vector<4> polarisation // (I, Q, U, V) Stoke's vector
            );

        // Constructor for specific predefined particle types in particle database
        explicit Particle(
            std::string type,
            const Vector<4> &position = Vector<4>(),    // (ct, x, y, z)
            const Vector<4> &momentum = Vector<4>(),    // (E/c, p_x, p_y, p_z)
            const Vector<4> &polarisation = Vector<4>() // (I, Q, U, V) Stoke's vector
            );

        // Getters
        [[nodiscard]] const std::string& getType() const;
        [[nodiscard]] const std::string& getSymbol() const;
        [[nodiscard]] const Quantity& getRestMass() const;
        [[nodiscard]] const Quantity& getCharge() const;
        [[nodiscard]] const Quantity& getSpin() const;
        [[nodiscard]] const Vector<4>& getPosition() const;
        [[nodiscard]] const Vector<4>& getMomentum() const;
        [[nodiscard]] const Vector<4>& getPolarisation() const;

        // Setters
        void setType(std::string type);
        void setSymbol(std::string symbol);
        void setRestMass(Quantity restMass);
        void setCharge(Quantity charge);
        void setSpin(Quantity spin);
        void setPosition(const Vector<4> &position);
        void setMomentum(const Vector<4> &momentum);
        void setPolarisation(const Vector<4> &polarisation);

        // Lorentz factor method
        [[nodiscard]] Quantity gamma() const;

        // Massless check method
        [[nodiscard]] bool isMassless() const;

        // Print method
        void print() const;

    private:
        std::string m_type;
        std::string m_symbol;
        Quantity m_restMass;
        Quantity m_charge;
        Quantity m_spin;
        Vector<4> m_position; // (ct, x, y, z)
        Vector<4> m_momentum; // (E/c, p_x, p_y, p_z)
        Vector<4> m_polarisation; // (I, Q, U, V) Stoke's vector
};

// ParticleManager
//
// Notes on initialisation:
//
// Notes on algorithms:
//
// Notes on output:
//
// Supported overloads / operations and functions / methods:
//
// Example usage:
class ParticleManager {
    public:
        ParticleManager() = default;

        // Add a single particle by forwarding construction arguments
        template<typename... Args>
        void addParticle(Args&&... args) {
            m_particles.push_back(std::make_unique<Particle>(std::forward<Args>(args)...));
        }

        // Add multiple particles from a vector
        void addParticles(const std::vector<Particle>& newParticles) {
            for (const auto& p : newParticles) {
                m_particles.push_back(std::make_unique<Particle>(p));
            }
        }

        std::vector<std::unique_ptr<Particle>>& getParticles() { return m_particles; }

        [[nodiscard]] bool empty() const { return m_particles.empty(); }

    private:
        std::vector<std::unique_ptr<Particle>> m_particles;
};

inline ParticleManager particleManager;

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
