//
// Physics Simulation Program
// File: step_utilities.cpp
// Created by Tobias Sharman on 16/11/2025
//
// Description:
//   - Implementation of step_utilities.h
//

#include "simulation/stepping/step_utilities.h"

#include <stdexcept>

#include "objects/object_manager.h"
#include "simulation/data-collection/particle_collection.h"

namespace step_utilities {
    void validateDetector(const Object *detector, const Object *world) {
        if (detector == nullptr) {
            throw std::invalid_argument("Cannot step particles because the detector pointer is null");
        }

        if (world == nullptr) {
            throw std::invalid_argument("Cannot step particles because the active world is null");
        }

        if (!ObjectManager::objectBelongsToWorld(detector, world)) {
            throw std::invalid_argument("Detector does not belong to the active world");
        }
    }

    const Object *resolveContainingMedium(const Object *world, const Vector<3> &position) {
        if (world == nullptr) {
            throw std::runtime_error("Active world is not available for locating containing medium");
        }

        return world->findObjectContaining(position);
    }

    const Object *resolveMediumIfAvailable(const Object *world, const Vector<3> &position) noexcept {
        return world != nullptr ? world->findObjectContaining(position) : nullptr;
    }

    void resetInteractionOnMediumChange(Particle &particle, const Object *previousMedium, const Object *currentMedium) {
        if (previousMedium != currentMedium) {
            particle.clearInteractionLength();
        }
    }

    bool updatePostEventState(
        std::unique_ptr<Particle> &particle,
        const Object *detector,
        const Object *previousMedium,
        const Object *currentMedium
    ) {
        if (!particle || !particle->getAlive()) { return false; }

        particle->pruneInteractionAndDecayProcesses();
        resetInteractionOnMediumChange(*particle, previousMedium, currentMedium);
        return logDetectorHit(particle, detector);
    }

    bool ensureParticleInsideWorld(Particle &particle, const Object *world) {
        if (world == nullptr) {
            throw std::runtime_error("Active world is not available for boundary check");
        }

        if (!world->contains(particle.getPosition())) {
            particle.setAlive(false);
            return false;
        }

        return true;
    }

    bool logDetectorHit(std::unique_ptr<Particle> &particle, const Object *detector) {
        logEnergyIfInside(particle, detector);
        return static_cast<bool>(particle);
    }

    void purgeDeadParticles(std::vector<std::unique_ptr<Particle> > &particles) {
        std::erase_if(
            particles,
            [](const std::unique_ptr<Particle> &particle) { return !particle || !particle->getAlive(); }
        );
    }
} // namespace step_utilities