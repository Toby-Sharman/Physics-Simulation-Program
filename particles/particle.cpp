//
// Created by Tobias Sharman on 28/08/2025.
//

#include "particle.h"

#include "vector.h"

#include "particle_database.h"

#include <iostream>
#include <string>
#include <utility>

// Empty construction
Particle::Particle()
: m_particleName("Unknown"),
  m_restMass(Quantity(0.0, "kg")),
  m_charge(Quantity(0.0, "e")),
  m_spin(Quantity(0.0, "ℏ")),
  m_position(Quantity(0.0, ""), Quantity(0.0, ""), Quantity(0.0, ""), Quantity(0.0, "")),
  m_momentum(Quantity(0.0, ""), Quantity(0.0, ""), Quantity(0.0, ""), Quantity(0.0, "")),
  m_polarisation(Quantity(0.0, ""), Quantity(0.0, ""), Quantity(0.0, ""))
{}

// Constructor for custom particles - have to assign basic attributes
Particle::Particle(
    std::string particleName,
    std::string symbol,
    Quantity restMass,
    Quantity charge,
    Quantity spin,
    Vector<4> position,
    Vector<4> momentum,
    Vector<3> polarisation)
: m_particleName(std::move(particleName)),
  m_symbol(std::move(symbol)),
  m_restMass(std::move(restMass)),
  m_charge(std::move(charge)),
  m_spin(std::move(spin)),
  m_position(std::move(position)),
  m_momentum(std::move(momentum)),
  m_polarisation(std::move(polarisation))
{}

// Constructor for specific predefined particle types in db
Particle::Particle(std::string particleName,
                   Vector<4> position,
                   Vector<4> momentum,
                   Vector<3> polarisation)
: m_particleName(std::move(particleName)),
  m_position(std::move(position)),
  m_momentum(std::move(momentum)),
  m_polarisation(std::move(polarisation))
{
    m_symbol = ParticleDB::getSymbol(particleName);
    m_restMass = ParticleDB::getRestMass(particleName);
    m_charge = Quantity(ParticleDB::getCharge(particleName), "e");
    m_spin = Quantity(ParticleDB::getSpin(particleName), "ℏ");
}

// Getters
const std::string &Particle::getParticleName() const { return m_particleName; }
const std::string &Particle::getSymbol() const { return m_symbol; }
Quantity Particle::getRestMass() const { return m_restMass; }
Quantity Particle::getCharge() const { return m_charge; }
Quantity Particle::getSpin() const { return m_spin; }
Vector<4> Particle::getPosition() const { return m_position; }
Vector<4> Particle::getMomentum() const { return m_momentum; }
Vector<3> Particle::getPolarisation() const { return m_polarisation; }


// Setters
void Particle::setParticleName(std::string particleName) { m_particleName = std::move(particleName); }
void Particle::setSymbol(std::string symbol) { m_symbol = std::move(symbol); }
void Particle::setRestMass(Quantity restMass) { m_restMass = std::move(restMass); }
void Particle::setCharge(Quantity charge) { m_charge = std::move(charge); }
void Particle::setSpin(Quantity spin) { m_spin = std::move(spin); }
void Particle::setPosition(Vector<4> position) { m_position = std::move(position); }
void Particle::setMomentum(Vector<4> momentum) { m_momentum = std::move(momentum); }
void Particle::setPolarisation(Vector<3> polarisation) { m_polarisation = std::move(polarisation); }

// Print function
void Particle::print() const {
    std::cout << "Particle Type: " << m_particleName << "\n";
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
    if (isMassless()) return Quantity(1.0, "kg"); // Not used for photons
    return Quantity(m_momentum[0].asDouble() / m_restMass.asDouble(), "FIXME"); // Using natural units // FIXME
}
