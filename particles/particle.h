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

#include "vector.h"

#include <string>

class Particle {
    public:
    Particle();
    Particle(
        std::string  particle_type,
        double rest_mass, // In MeV/c^2
        double charge,
        double spin,
        const Vector<4>& position, // (ct, x, y, z) In Mev^-1, MeV/c
        const Vector<4>& momentum, // (E/c, p_x, p_y, p_z) In MeV
        const Vector<3>& polarisation // (P_x, P_y, P_z)
        );

    // Getters
    [[nodiscard]] std::string getParticleType() const;
    [[nodiscard]] double getRestMass() const;
    [[nodiscard]] double getCharge() const;
    [[nodiscard]] double getSpin() const;
    [[nodiscard]] const Vector<4>& getPosition() const;
    [[nodiscard]] const Vector<4>& getMomentum() const;
    [[nodiscard]] const Vector<3>& getPolarisation() const;


    // Setters
    void setParticleType(const std::string& particle_type);
    void setRestMass(double rest_mass);
    void setCharge(double charge);
    void setSpin(double spin);
    void setPosition(const Vector<4>& position);
    void setMomentum(const Vector<4>& momentum);
    void setPolarisation(const Vector<3>& polarisation);

    // Print function
    void print() const;

    // Lorentz factor
    // Not used to update particle position
    // Would be used to multiply interaction probabilities if using monte carlo
    [[nodiscard]] double gamma() const;

    // Check for if the particle is massless or practically massless
    [[nodiscard]] bool isMassless() const { return std::abs(m_rest_mass) <= Globals::Constant::MASSLESS_TOLERANCE; }

    private:
    std::string m_particle_type;
    double m_rest_mass; // In eV
    double m_charge;
    double m_spin;
    Vector<4> m_position; // (ct, x, y, z)
    Vector<4> m_momentum; // (E/c, p_x, p_y, p_z)
    Vector<3> m_polarisation; // (P_x, P_y, P_z)
};

#endif //PHYSICS_SIMULATION_PROGRAM_PARTICLES_H
