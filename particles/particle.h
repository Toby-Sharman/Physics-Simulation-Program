////////////////////////////////////////////////////////////////////////////////////////////////////
// File:        Particles.h                                                                       //
// Author:      Tobias Sharman                                                                    //
// Created:     28/08/2025                                                                        //
// Last Modified: 29/08/2025
// Description: Declarations for the Particles module                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLES_H
#define PARTICLES_H

#include "globals.h"

#include "vector.h"

#include <iostream>
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
    [[nodiscard]] std::string get_particle_type() const;
    [[nodiscard]] double get_rest_mass() const;
    [[nodiscard]] double get_charge() const;
    [[nodiscard]] double get_spin() const;
    [[nodiscard]] const Vector<4>& get_position() const;
    [[nodiscard]] const Vector<4>& get_momentum() const;
    [[nodiscard]] const Vector<3>& get_polarisation() const;


    // Setters
    void set_particle_type(const std::string& particle_type);
    void set_rest_mass(double rest_mass);
    void set_charge(double charge);
    void set_spin(double spin);
    void set_position(const Vector<4>& position);
    void set_momentum(const Vector<4>& momentum);
    void set_polarisation(const Vector<3>& polarisation);

    // Print function
    void print() const;

    // Lorentz factor
    // Not used to update particle position
    // Would be used to multiply interaction probabilities if using monte carlo
    [[nodiscard]] double gamma() const;

    // Check for if the particle is massless or practically massless
    [[nodiscard]] bool is_massless() const { return std::abs(m_rest_mass) <= Globals::Constant::MASSLESS_TOLERANCE; }

    private:
    std::string m_particle_type;
    double m_rest_mass; // In eV
    double m_charge;
    double m_spin;
    Vector<4> m_position; // (ct, x, y, z)
    Vector<4> m_momentum; // (E/c, p_x, p_y, p_z)
    Vector<3> m_polarisation; // (P_x, P_y, P_z)
};

#endif //PARTICLES_H
