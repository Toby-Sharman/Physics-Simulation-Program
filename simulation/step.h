//
// Created by Tobias Sharman on 01/09/2025.
//

#ifndef STEP_H
#define STEP_H

#include "globals.h"
#include "particle.h"


void step(Particle& p, double dt = Globals::Constant::TIME_STEP);

#endif //STEP_H
