////////////////////////////////////////////////////////////////////////////////////////////////////
// File:          Particles.cpp                                                                   //
// Author:        Tobias Sharman                                                                  //
// Created:       28/08/2025                                                                      //
// Last Modified: 29/08/2025                                                                      //
// Description:   Implementation of the Particles module                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TODO: Shift all particle attributes with units to vector data type
// TODO: Change particle labelling to units not type, i.e. m not x, y, or z
// TODO: Could shift trivial getters and setters to in just header (with inline)

#include "vector.h"

#include "particle.h"

#include <iostream>
#include <string>
#include <utility>

// Default constructor with proper physics labels
Particle::Particle()
: m_particle_type("Unknown"),
  m_rest_mass(0.0),
  m_charge(0.0),
  m_spin(0.0),
  m_position(Data({0.0, 0.0, 0.0, 0.0}), Labels({"ct", "x", "y", "z"})),
  m_momentum(Data({0.0, 0.0, 0.0, 0.0}), Labels({"E/c", "p_x", "p_y", "p_z"})),
  m_polarisation(Data({0.0, 0.0, 0.0}), Labels({"P_x", "P_y", "P_z"}))
{}

// Parameterized constructor
Particle::Particle(
    std::string particle_type,
    double rest_mass,
    double charge,
    double spin,
    const Vector<4>& position,
    const Vector<4>& momentum,
    const Vector<3>& polarisation)
: m_particle_type(std::move(particle_type)),
  m_rest_mass(rest_mass),
  m_charge(charge),
  m_spin(spin),
  m_position(position), // TODO: Add auto label assignment if unspecified - will require two functions one with just data and one for data and values, also have check for if allowed values are of the correct type
  m_momentum(momentum),
  m_polarisation(polarisation)
{}

// Getters
std::string Particle::get_particle_type() const { return m_particle_type; }
double Particle::get_rest_mass() const { return m_rest_mass; }
double Particle::get_charge() const { return m_charge; }
double Particle::get_spin() const { return m_spin; }
const Vector<4>& Particle::get_position() const { return m_position; }
const Vector<4>& Particle::get_momentum() const { return m_momentum; }
const Vector<3>& Particle::get_polarisation() const { return m_polarisation; }


// Setters
void Particle::set_particle_type(const std::string& particle_type) { m_particle_type = particle_type; }
void Particle::set_rest_mass(double rest_mass) { m_rest_mass = rest_mass; }
void Particle::set_charge(double charge) { m_charge = charge; }
void Particle::set_spin(double spin) { m_spin = spin; }
void Particle::set_position(const Vector<4>& position) { m_position = position; } // TODO: Extend initial assignment label handling to setter functions
void Particle::set_momentum(const Vector<4>& momentum) { m_momentum = momentum; }
void Particle::set_polarisation(const Vector<3>& polarisation) { m_polarisation = polarisation; }

// Print function
void Particle::print() const {
    std::cout << "Particle Type: " << m_particle_type << "\n";
    std::cout << "Rest Mass: " << m_rest_mass << "\n";
    std::cout << "Charge: " << m_charge << "\n";
    std::cout << "Spin: " << m_spin << "\n";
    std::cout << "Position: ";
    m_position.print();
    std::cout << "Momentum: ";
    m_momentum.print();
    std::cout << "Polarisation: ";
    m_polarisation.print();
}

// Lorentz factor
double Particle::gamma() const {
    if (is_massless()) return 1.0; // Not used for photons
    return m_momentum[0] / m_rest_mass; // assuming units c=1
}
