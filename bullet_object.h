#pragma once
#ifndef BULLETOBJECT_H_
#define BULLETOBJECT_H_

#include <math.h>

#include "game_object.h"
#include "physical_object.h"
#include "timed_object.h"   
#include "particle_system.h"

namespace game {

    // Inherits from GameObject
    class BulletObject : public TimedObject {

    public:
        BulletObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float direction, glm::vec3 speed, double ttd);
        ~BulletObject();
        void Update(double delta_time) override;
        void Render(glm::mat4 view_matrix, double current_time) override;
        void speedUp(double delta_time);
        void bindParticleSystem(ParticleSystem*);
        // Update function for moving the player object around
    protected:
        ParticleSystem* myParticles;
    }; // class PhysicalObject

} // namespace game

#endif // TIMEDOBJECT_H_