//
// Physics Simulation Program
// File: particle_motion.cpp
// Created by Tobias Sharman on 15/11/2025
//
// Description:
//   - Implementation of particle_motion.h
//

#include "simulation/motion/particle_motion.h"

void moveParticle(Particle& particle, const Quantity& dt, const Vector<3>& displacement) {
    const auto updatedPosition = particle.getPosition() + displacement;
    const auto updatedTime = particle.getTime() + dt;
    particle.setPosition(updatedTime, updatedPosition);
}