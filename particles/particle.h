////////////////////////////////////////////////////////////////////////////////////////////////////
// File:        Particles.h                                                                       //
// Author:      Tobias Sharman                                                                    //
// Created:     28/08/2025                                                                        //
// Last Modified: 29/08/2025
// Description: Declarations for the Particles module                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
#define PHYSICS_SIMULATION_PROGRAM_PARTICLES_H

#include "globals.h"

#include "quantity.h"
#include "vector.h"

#include <string>

class Particle {
    public:
    // Empty constructor
    Particle();

    // Constructor for custom particles - have to assign basic attributes
    Particle(
        std::string particleName,
        std::string symbol,
        Quantity restMass,
        Quantity charge,
        Quantity spin,
        Vector<4> position,    // (ct, x, y, z)
        Vector<4> momentum,    // (E/c, p_x, p_y, p_z)
        Vector<3> polarisation // (P_x, P_y, P_z)
        );

    // Constructor for specific predefined particle types in db
    explicit Particle(
        std::string particleName,
        Vector<4> position = Vector<4>(),    // (ct, x, y, z)
        Vector<4> momentum = Vector<4>(),    // (E/c, p_x, p_y, p_z)
        Vector<3> polarisation = Vector<3>() // (P_x, P_y, P_z)
        );

    // Destructor
    ~Particle() = default; // Default deconstruction

    // Getters
    // Return strings by const value as they are passed by reference and should not be changed
    // Return quantities and vectors by a copy as they are relatively small and will likely need to be used in calculations
    //      Safer than direct alteration with pass by reference
    [[nodiscard]] const std::string& getParticleName() const; // Using "const &" keeps memory usage down as getName may be called lots
    [[nodiscard]] const std::string& getSymbol() const; // Keep string convention consistent
    [[nodiscard]] Quantity getRestMass() const;
    [[nodiscard]] Quantity getCharge() const;
    [[nodiscard]] Quantity getSpin() const;
    [[nodiscard]] Vector<4> getPosition() const;
    [[nodiscard]] Vector<4> getMomentum() const;
    [[nodiscard]] Vector<3> getPolarisation() const;

    // Setters
    // Pass copies of inputs for all types. Using move will keep overhead close to const reference
    void setParticleName(std::string particleName);
    void setSymbol(std::string symbol);
    void setRestMass(Quantity restMass);
    void setCharge(Quantity charge);
    void setSpin(Quantity spin);
    void setPosition(Vector<4> position);
    void setMomentum(Vector<4> momentum);
    void setPolarisation(Vector<3> polarisation);

    // Print function
    void print() const;

    // Lorentz factor
    // Not used to update particle position
    // Would be used to multiply interaction probabilities if using monte carlo
    [[nodiscard]] Quantity gamma() const;

    // Check for if the particle is massless or practically massless
    [[nodiscard]] bool isMassless() const { return std::abs(m_restMass.asDouble()) <= Globals::Constant::MASSLESS_TOLERANCE; }

    private:
    std::string m_particleName;
    std::string m_symbol;
    Quantity m_restMass;
    Quantity m_charge;
    Quantity m_spin;
    Vector<4> m_position; // (ct, x, y, z)
    Vector<4> m_momentum; // (E/c, p_x, p_y, p_z)
    Vector<3> m_polarisation; // (P_x, P_y, P_z)
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
