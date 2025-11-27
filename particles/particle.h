//
// Physics Simulation Program
// File: particle.h
// Created by Tobias Sharman on 28/08/2025
//
// Description:
//   - Describe the base particle class
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLES_H

#include <iosfwd>
#include <string>
#include <utility>

#include "config/program_config.h"
#include "core/linear-algebra/vector.h"
#include "core/quantities/quantity.h"

namespace discrete_interaction {
    class InteractionProcess;
}

// Particle
//
// Notes on initialisation:
//   - Can be constructed via specified arguments for all particle fields or via a particle type and its position,
//     momentum, and polarisation.
//         -> If constructing via the particle type it must be described in the particle database
//   - Recommended to construct particles via particle source not directly from here
//
// Notes on algorithms:
//
// Notes on output:
//   - print() emits a basic textual dump of scalar/vector state followed by printPolarisation()
//
// Supported overloads / operations and functions / methods:
//   - Constructors:           Particle()
//   - Getters:                get_____() (Alive, Type, Symbol, RestMass, Charge, Spin, Time, Position, Energy,
//                                         Momentum, Speed, Lifetime)
//   - Decay helpers:          hasDecayClock(), getDecayTimeRemaining(), hasDecayEnergy(), getDecayEnergy(),
//                             setDecayClock(), consumeDecayTime(), clearDecayClock(), setDecayEnergy(),
//                             clearDecayEnergy(), clearDecayState()
//   - Interaction helpers:    hasPendingInteractionLength(), getInteractionLengthRemaining(),
//                             getPendingInteractionProcess(), setInteractionLengthRemaining(),
//                             consumeInteractionLength(), clearInteractionLength()
//   - Setters:                set_____() (Alive, Type, Symbol, RestMass, Charge, Spin, Position, Momentum, Lifetime)
//   - Spatial helpers:        pruneInteractionAndDecayProcesses(), synchronizeTime(),
//                             reflectMomentumAcrossNormal(), isReflective()
//   - Physics helpers:        gamma(), isMassless()
//   - Lifetime control:       kill()
//   - Output:                 print()
//
// Example usage: TODO
class [[nodiscard]] Particle {
    public:
        virtual ~Particle() = default;

        // Constructor for custom particles
        Particle(
            std::string type,
            std::string symbol,
            Quantity restMass,
            Quantity charge,
            Quantity spin,
            Quantity time,
            const Vector<3> &position,
            Quantity energy,
            const Vector<3> &momentum
            );

        // Constructor for specific predefined particle types in particle database
        explicit Particle(
            std::string type,
            const Quantity& time = Quantity(),
            const Vector<3>& position = Vector<3>(),
            const Quantity& energy = Quantity(),
            const Vector<3>& momentum = Vector<3>()
            );

        // Getters
        [[nodiscard]] constexpr bool getAlive() const noexcept { return this->m_alive; }
        [[nodiscard]] constexpr const std::string& getType() const noexcept { return this->m_type; }
        [[nodiscard]] constexpr const std::string& getSymbol() const noexcept { return this->m_symbol; }
        [[nodiscard]] constexpr const Quantity& getRestMass() const noexcept { return this->m_restMass; }
        [[nodiscard]] constexpr const Quantity& getCharge() const noexcept { return this->m_charge; }
        [[nodiscard]] constexpr const Quantity& getSpin() const noexcept { return this->m_spin; }
        [[nodiscard]] constexpr const Quantity& getTime() const noexcept { return this->m_time; }
        [[nodiscard]] constexpr const Vector<3>& getPosition() const noexcept { return this->m_position; }
        [[nodiscard]] constexpr const Quantity& getEnergy() const noexcept { return this->m_energy; }
        [[nodiscard]] constexpr const Vector<3>& getMomentum() const noexcept { return this->m_momentum; }
        [[nodiscard]] Quantity getSpeed() const; // Computes relativistic speed
        [[nodiscard]] constexpr const Quantity& getLifetime() const noexcept { return this->m_lifetime; }
        [[nodiscard]] constexpr bool hasDecayClock() const noexcept { return this->m_hasDecayClock; }
        [[nodiscard]] constexpr const Quantity& getDecayTimeRemaining() const noexcept { return this->m_timeUntilDecay; }
        [[nodiscard]] constexpr bool hasDecayEnergy() const noexcept { return this->m_hasDecayEnergy; }
        [[nodiscard]] constexpr const Quantity& getDecayEnergy() const noexcept { return this->m_decayEnergy; }

        // Setters
        constexpr void setAlive(const bool alive) noexcept { this->m_alive = alive; }
        void setType(std::string type) noexcept { this->m_type = std::move(type); }
        void setSymbol(std::string symbol) noexcept { this->m_symbol = std::move(symbol); }
        void setRestMass(Quantity restMass); // Dimension enforcement
        void setCharge(Quantity charge); // Dimension enforcement
        void setSpin(Quantity spin); // Dimension enforcement
        void setTime(const Quantity& time); // Dimension enforcement
        void setPosition(const Vector<3>& position); // Dimension enforcement
        void setEnergy(const Quantity& energy); // Dimension enforcement
        void setMomentum(const Vector<3>& momentum); // Dimension enforcement
        void setLifetime(const Quantity& lifetime); // Dimension enforcement
        void clearDecayClock(); // Update hasDecayClock and timeUntilDecay
        void setDecayClock(const Quantity& decayTime); // Dimension enforcement; update hasDecayClock and timeUntilDecay
        void consumeDecayTime(const Quantity& dt); // Dimension enforcement; update timeUntilDecay
        void setDecayEnergy(const Quantity& energy); // Dimension enforcement; update hasDecayEnergy and decayEnergy
        void clearDecayEnergy(); // Update hasDecayEnergy and decayEnergy
        void clearDecayState(); // Run clearDecayClock and clearDecayEnergy

        // Discrete interaction bookkeeping
        [[nodiscard]] constexpr bool hasPendingInteractionLength() const noexcept { return this->m_hasPendingInteractionLength; }
        [[nodiscard]] constexpr const Quantity& getInteractionLengthRemaining() const noexcept { return this->m_interactionLengthRemaining; }
        [[nodiscard]] constexpr const discrete_interaction::InteractionProcess* getPendingInteractionProcess() const noexcept { return this->m_pendingInteractionProcess; }
        // Dimension enforcement; update interactionLengthRemaining, pendingInteractionProcess, and hasPendingInteractionLength
        void setInteractionLengthRemaining(const Quantity& length, const discrete_interaction::InteractionProcess* process);
        void consumeInteractionLength(const Quantity& lengthTravelled); // Dimension enforcement; update interactionLengthRemaining
        void clearInteractionLength(); // Update interactionLengthRemaining, pendingInteractionProcess, and hasPendingInteractionLength

        // Reflection helpers
        [[nodiscard]] bool isReflective() const noexcept { return getType() != "photon"; }
        void reflectMomentumAcrossNormal(const Vector<3>& normal); // Updates momentum; works on assumption that normal is a unit vector

        // Time/interaction bookkeeping
        void pruneInteractionAndDecayProcesses(); // Sort out interaction length and decay time for if <= 0
        // Ensure time step advancement is within tolerance and not skewed by step limiters
        void synchroniseTime(const Quantity& expectedTime, double toleranceFactor = config::program::timeSynchronisationTolerance);

        // Lorentz factor method
        [[nodiscard]] Quantity gamma() const;

        // Massless check method
        [[nodiscard]] bool isMassless() const;

        // Mark for deletion method
        void kill() noexcept { this->m_alive = false; }

        // Print method
        void print() const;

    private:
        bool m_alive = true;
        std::string m_type;
        std::string m_symbol;
        Quantity m_restMass;
        Quantity m_charge;
        Quantity m_spin;
        Quantity m_time; // t
        Vector<3> m_position; // (x, y, z)
        Quantity m_energy; // E
        Vector<3> m_momentum; // (p_x, p_y, p_z)
        Quantity m_lifetime = Quantity::dimensionless(0.0);

        bool m_hasPendingInteractionLength = false;
        const discrete_interaction::InteractionProcess* m_pendingInteractionProcess = nullptr;
        Quantity m_interactionLengthRemaining = Quantity();
        bool m_hasDecayClock = false;
        Quantity m_timeUntilDecay = Quantity();
        bool m_hasDecayEnergy = false;
        Quantity m_decayEnergy = Quantity();

    protected:
        virtual void printPolarisation(std::ostream& stream) const;
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
