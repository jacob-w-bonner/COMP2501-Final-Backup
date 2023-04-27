#pragma once
#ifndef ENEMYBULLETOBJECT_H_
#define ENEMYBULLETOBJECT_H_

#include <math.h>

#include "game_object.h"
#include "timed_object.h"   
#include "particle_system.h"

namespace game {

    // Inherits from GameObject
    class EnemyBulletObject : public TimedObject {

    public:
        EnemyBulletObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float direction, float speed, double ttd);
        ~EnemyBulletObject();
        void Update(double delta_time) override;
        void Render(glm::mat4 view_matrix, double current_time) override;
        void speedUp(double delta_time);
        bool checkIsOffScreen();
        void bindParticleSystem(ParticleSystem*);
        bool handleCollision(glm::vec3 targetPosition, glm::vec3 targetScale);
        // Update function for moving the player object around
    protected:
        ParticleSystem* myParticles;
    }; // class PhysicalObject

} // namespace game

#endif // TIMEDOBJECT_H_