//
// Created by Tobias Sharman on 28/08/2025.
//

#include "particle.h"

#include "vector.h"

#include "particle_database.h"

#include <iostream>
#include <string>
#include <utility>

// Constructor for custom particles - have to assign basic attributes
Particle::Particle(
    std::string type,
    std::string symbol,
    Quantity restMass,
    Quantity charge,
    Quantity spin,
    Vector<4> position,
    Vector<4> momentum,
    Vector<3> polarisation)
: m_type(std::move(type)),
  m_symbol(std::move(symbol)),
  m_restMass(restMass),
  m_charge(charge),
  m_spin(spin),
  m_position(position),
  m_momentum(momentum),
  m_polarisation(polarisation)
{}

// Constructor for specific predefined particle types in db
Particle::Particle(std::string type,
                   const Vector<4> &position,
                   const Vector<4> &momentum,
                   const Vector<3> &polarisation)
: m_type(std::move(type)),
  m_position(position),
  m_momentum(momentum),
  m_polarisation(polarisation)
{
    m_symbol = particleDatabase.getSymbol(m_type);
    m_restMass = particleDatabase.getRestMass(m_type);
    m_charge = Quantity(particleDatabase.getCharge(m_type), "e");
    m_spin = Quantity(particleDatabase.getSpin(m_type), "ℏ");
}

// Getters
const std::string& Particle::getType() const { return m_type; }
const std::string& Particle::getSymbol() const { return m_symbol; }
const Quantity& Particle::getRestMass() const { return m_restMass; }
const Quantity& Particle::getCharge() const { return m_charge; }
const Quantity& Particle::getSpin() const { return m_spin; }
const Vector<4>& Particle::getPosition() const { return m_position; }
const Vector<4>& Particle::getMomentum() const { return m_momentum; }
const Vector<3>& Particle::getPolarisation() const { return m_polarisation; }


// Setters
void Particle::setType(std::string type) { m_type = std::move(type); }
void Particle::setSymbol(std::string symbol) { m_symbol = std::move(symbol); }
void Particle::setRestMass(const Quantity restMass) { m_restMass = restMass; }
void Particle::setCharge(const Quantity charge) { m_charge = charge; }
void Particle::setSpin(const Quantity spin) { m_spin = spin; }
void Particle::setPosition(const Vector<4> &position) { m_position = position; }
void Particle::setMomentum(const Vector<4> &momentum) { m_momentum = momentum; }
void Particle::setPolarisation(const Vector<3> &polarisation) { m_polarisation = polarisation; }

// Print function
void Particle::print() const {
    std::cout << "Particle Type: " << m_type << "\n";
    std::cout << "Rest Mass: " << m_restMass << "\n";
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
Quantity Particle::gamma() const {
    if (isMassless()) {
        return {1.0, "kg"}; // Not used for photons
    }
    return {m_momentum[0] / m_restMass}; // Using natural units // TODO CHECK
}

bool Particle::isMassless() const {
    return m_restMass.abs() <= quantityTable().at("massless tolerance");
}
