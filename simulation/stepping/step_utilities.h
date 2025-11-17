//
// Physics Simulation Program
// File: step_utilities.h
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Utility helpers shared by the simulation step routines
//

#ifndef PHYSICS_SIMULATION_PROGRAM_STEP_UTILITIES_H
#define PHYSICS_SIMULATION_PROGRAM_STEP_UTILITIES_H

#include <memory>
#include <vector>

#include "core/linear-algebra/vector.h"
#include "objects/object.h"
#include "particles/particle.h"

namespace step_utilities {
    // Validate detector pointers before using them inside the step loop
    void validateDetector(const Object *detector, const Object *world);

    // Locate the medium that contains the given position, using the provided context string
    const Object *resolveContainingMedium(const Object *world, const Vector<3> &position);

    // Locate the medium if possible, otherwise return nullptr when the world is absent
    const Object *resolveMediumIfAvailable(const Object *world, const Vector<3> &position) noexcept;

    // Clear pending interaction data whenever the particle transitions into a different medium
    void resetInteractionOnMediumChange(Particle &particle, const Object *previousMedium, const Object *currentMedium);

    // Prune expired interaction/decay timers, reset sampling when mediums change, and log detector hits
    bool updatePostEventState(
        std::unique_ptr<Particle> &particle,
        const Object *detector,
        const Object *previousMedium,
        const Object *currentMedium
    );

    // Verify particles remain in the active world
    bool ensureParticleInsideWorld(Particle &particle, const Object *world);

    // Detector bookkeeping helper
    bool logDetectorHit(std::unique_ptr<Particle> &particle, const Object *detector);

    // Collection helpers
    void purgeDeadParticles(std::vector<std::unique_ptr<Particle> > &particles);
} // namespace step_utilities

#endif //PHYSICS_SIMULATION_PROGRAM_STEP_UTILITIES_H