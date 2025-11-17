//
// Physics Simulation Program
// File: particle.cpp
// Created by Tobias Sharman on 28/08/2025
//
// Description:
//   - Implementation of particle.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "particles/particle.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "databases/particle-data/particle_database.h"

// Constructor for custom particles - have to assign basic attributes
Particle::Particle(
    std::string type,
    std::string symbol,
    const Quantity restMass,
    const Quantity charge,
    const Quantity spin,
    const Quantity time,
    const Vector<3>& position,
    const Quantity energy,
    const Vector<3>& momentum)
{
    this->setType(std::move(type));
    this->setSymbol(std::move(symbol));
    this->setRestMass(restMass);
    this->setCharge(charge);
    this->setSpin(spin);
    this->setTime(time);
    this->setPosition(position);
    this->setEnergy(energy);
    this->setMomentum(momentum);
    this->setLifetime(Quantity::dimensionless(0.0));
    this->clearDecayState();
}

// Constructor for specific predefined particle types in db
Particle::Particle(std::string type,
                   const Quantity& time,
                   const Vector<3>& position,
                   const Quantity& energy,
                   const Vector<3>& momentum)
{
    this->setType(std::move(type));
    this->setTime(time);
    this->setPosition(position);
    this->setEnergy(energy);
    this->setMomentum(momentum);
    this->setSymbol(g_particleDatabase.getSymbol(this->m_type));
    this->setRestMass(g_particleDatabase.getRestMass(this->m_type));
    this->setCharge(g_particleDatabase.getCharge(this->m_type));
    this->setSpin(g_particleDatabase.getSpin(this->m_type));
    this->setLifetime(g_particleDatabase.getLifetime(this->m_type));
    this->clearDecayState();
}

Quantity Particle::getSpeed() const {
    const auto c = quantityTable().at("c");
    if (isMassless()) {
        return c;
    }

    const auto momentumMagnitude = this->m_momentum.length();
    if (this->m_energy.value == 0.0) {
        return Quantity::dimensionless(0.0);
    }

    return momentumMagnitude / this->m_energy * c * c;
}

void Particle::setRestMass(Quantity restMass) {
    if (!Unit::hasMassDimension(restMass.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' rest mass must have units {} but got {}",
            this->m_type,
            Unit::massDimension().toString(),
            restMass
        ));
    }
    this->m_restMass = restMass;
}

void Particle::setCharge(Quantity charge) {
    if (!Unit::hasElectricChargeDimension(charge.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' charge must have units {} but got {}",
            this->m_type,
            Unit::electricChargeDimension().toString(),
            charge
        ));
    }
    this->m_charge = charge;
}

void Particle::setSpin(Quantity spin) {
    if (!Unit::hasAngularMomentumDimension(spin.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' spin must have units {} but got {}",
            this->m_type,
            Unit::angularMomentumDimension().toString(),
            spin
        ));
    }
    this->m_spin = spin;
}

void Particle::setTime(const Quantity& time) {
    if (!Unit::hasTimeDimension(time.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' time must have units {} but got {}",
            this->m_type,
            Unit::timeDimension().toString(),
            time
        ));
    }
    this->m_time = time;
}

void Particle::setPosition(const Vector<3>& position) {
    for (std::size_t axis = 0; axis < 3; ++axis) {
        if (!Unit::hasLengthDimension(position[axis].unit)) {
            throw std::invalid_argument(std::format(
                "Particle '{}' position[{}] must have units {} but got {}",
                this->m_type,
                axis,
                Unit::lengthDimension().toString(),
                position[axis]
            ));
        }
    }
    this->m_position = position;
}

void Particle::setEnergy(const Quantity& energy) {
    if (!Unit::hasEnergyDimension(energy.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' energy must have units {} but got {}",
            this->m_type,
            Unit::energyDimension().toString(),
            energy
        ));
    }
    this->m_energy = energy;
}

void Particle::setMomentum(const Vector<3>& momentum) {
    for (std::size_t axis = 0; axis < 3; ++axis) {
        if (!Unit::hasMomentumDimension(momentum[axis].unit)) {
            throw std::invalid_argument(std::format(
                "Particle '{}' momentum[{}] must have units {} but got {}",
                this->m_type,
                axis,
                Unit::momentumDimension().toString(),
                momentum[axis]
            ));
        }
    }
    this->m_momentum = momentum;
}

void Particle::setLifetime(const Quantity& lifetime) {
    if (!Unit::hasTimeDimension(lifetime.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' lifetime must have units {} but got {}",
            this->m_type,
            Unit::timeDimension().toString(),
            lifetime
        ));
    }
    this->m_lifetime = lifetime;
    this->clearDecayClock();
}

void Particle::clearDecayClock() {
    this->m_hasDecayClock = false;
    this->m_timeUntilDecay = Quantity();
}

void Particle::setDecayClock(const Quantity& decayTime) {
    if (!Unit::hasTimeDimension(decayTime.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' decay clock must have units {} but got {}",
            this->m_type,
            Unit::timeDimension().toString(),
            decayTime
        ));
    }
    this->m_timeUntilDecay = decayTime;
    this->m_hasDecayClock = true;
}

void Particle::consumeDecayTime(const Quantity& dt) {
    if (!this->m_hasDecayClock) {
        return;
    }
    if (!Unit::hasTimeDimension(dt.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' decay clock decrement must have units {} but got {}",
            this->m_type,
            Unit::timeDimension().toString(),
            dt
        ));
    }
    this->m_timeUntilDecay -= dt;
    if (this->m_timeUntilDecay.value <= 0.0) {
        this->m_timeUntilDecay.value = 0.0;
    }
}

void Particle::setDecayEnergy(const Quantity& energy) {
    if (!Unit::hasEnergyDimension(energy.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' decay energy must have units {} but got {}",
            this->m_type,
            Unit::energyDimension().toString(),
            energy
        ));
    }
    this->m_decayEnergy = energy;
    this->m_hasDecayEnergy = true;
}

void Particle::clearDecayEnergy() {
    this->m_hasDecayEnergy = false;
    this->m_decayEnergy = Quantity();
}

void Particle::clearDecayState() {
    this->clearDecayClock();
    this->clearDecayEnergy();
}

void Particle::setInteractionLengthRemaining(const Quantity& length,
                                             const discrete_interaction::InteractionProcess* process)
{
    if (!Unit::hasLengthDimension(length.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' interaction length must have units {} but got {}",
            this->m_type,
            Unit::lengthDimension().toString(),
            length
        ));
    }
    if (!std::isfinite(length.value) || length.value <= 0.0 || process == nullptr) {
        clearInteractionLength();
        return;
    }
    this->m_interactionLengthRemaining = length;
    this->m_pendingInteractionProcess = process;
    this->m_hasPendingInteractionLength = true;
}

void Particle::consumeInteractionLength(const Quantity& lengthTravelled) {
    if (!this->m_hasPendingInteractionLength) {
        return;
    }
    if (!Unit::hasLengthDimension(lengthTravelled.unit)) {
        throw std::invalid_argument(std::format(
            "Particle '{}' interaction decrement must have units {} but got {}",
            this->m_type,
            Unit::lengthDimension().toString(),
            lengthTravelled
        ));
    }
    this->m_interactionLengthRemaining -= lengthTravelled;
    if (this->m_interactionLengthRemaining.value <= 0.0) {
        this->m_interactionLengthRemaining.value = 0.0;
    }
}

void Particle::clearInteractionLength() {
    this->m_hasPendingInteractionLength = false;
    this->m_pendingInteractionProcess = nullptr;
    this->m_interactionLengthRemaining = Quantity();
}

void Particle::reflectMomentumAcrossNormal(const Vector<3>& normal) {
    const auto& momentum = getMomentum();
    const auto newMomentum = momentum - 2 * momentum.dot(normal) * normal;
    setMomentum(newMomentum);
}

void Particle::pruneInteractionAndDecayProcesses() {
    if (this->m_hasPendingInteractionLength && this->m_interactionLengthRemaining.value <= 0.0) {
        this->clearInteractionLength();
    }

    if (this->m_hasDecayClock && this->m_timeUntilDecay.value <= 0.0) {
        this->clearDecayClock();
    }
}

void Particle::synchroniseTime(const Quantity& expectedTime, const double toleranceFactor) {
    if (const double tolerance = std::max(std::abs(expectedTime.value) * toleranceFactor, std::numeric_limits<double>::epsilon());
        std::abs(this->m_time.value - expectedTime.value) > tolerance) {

        this->setTime(expectedTime);
    }
}

Quantity Particle::gamma() const {
    if (isMassless()) {
        return Quantity::dimensionless(1.0);
    }
    const auto c = quantityTable().at("c");
    return this->m_energy / (this->m_restMass * c * c);
}

bool Particle::isMassless() const {
    return this->m_restMass.abs() <= quantityTable().at("massless tolerance");
}

void Particle::print() const { // TODO: UPDATE
    std::cout << "Particle Type: " << this->m_type << "\n";
    std::cout << "Rest Mass: " << this->m_restMass << "\n";
    std::cout << "Charge: " << this->m_charge << "\n";
    std::cout << "Spin: " << this->m_spin << "\n";
    std::cout << "Time: " << this->m_time << "\n";
    std::cout << "Position: ";
    this->m_position.print();
    std::cout << "Energy: " << this->m_energy << "\n";
    std::cout << "Momentum: ";
    this->m_momentum.print();
    this->printPolarisation(std::cout);
}

void Particle::printPolarisation(std::ostream& stream) const {
    stream << "Polarisation: (not tracked)\n";
}
