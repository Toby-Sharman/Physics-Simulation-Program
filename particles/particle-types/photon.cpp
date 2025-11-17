//
// Physics Simulation Program
// File: photon.cpp
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Implementation of photon.h
//
// Copyright (c) 2025, Tobias Sharman
// Licensed under a Non-Commercial License. See LICENSE file for details
//

#include "particles/particle-types/photon.h"

Photon::Photon(
    std::string type,
    std::string symbol,
    const Quantity restMass,
    const Quantity charge,
    const Quantity spin,
    const Quantity time,
    const Vector<3> &position,
    const Quantity energy,
    const Vector<3> &momentum,
    const Vector<4> &polarisation)
    : Particle(std::move(type), std::move(symbol), restMass, charge, spin, time, position, energy, momentum)
{
    this->setPolarisation(polarisation);
}

Photon::Photon(
    std::string type,
    const Quantity& time,
    const Vector<3>& position,
    const Quantity& energy,
    const Vector<3>& momentum,
    const Vector<4>& polarisation)
    : Particle(std::move(type), time, position, energy, momentum)
{
    this->setPolarisation(polarisation);
}

void Photon::printPolarisation(std::ostream& stream) const {
    stream << "Polarisation (I, Q, U, V): ";
    this->m_polarisation.print();
}