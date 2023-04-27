#include "enemy_bullet_object.h"
#include "game_object.h"
#include "math_object.h"

#include <math.h>
#include <iostream>

namespace game {

    /*
        physicalobject inherits from GameObject
        It implements Radius, which is used for collision detection.
    */

    EnemyBulletObject::EnemyBulletObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float direction, float newSpeed, double ttd)
        : TimedObject(position, geom, shader, texture, ttd)
    {
        //rotate_[0][0] = 3*3.14/2 + direction;
        velocity_ = MathObject::findVelocityFromAngleSpeed(direction, newSpeed);
        myParticles = NULL;
    }

    void EnemyBulletObject::bindParticleSystem(ParticleSystem* newParticles)
    {
        myParticles = newParticles;
    }

    void EnemyBulletObject::Update(double delta_time)
    {
        TimedObject::Update(delta_time);
        if (myParticles != NULL)
        {
            myParticles->Update(delta_time);
        }
    }

    void EnemyBulletObject::Render(glm::mat4 view_matrix, double current_time)
    {
        GameObject::Render(view_matrix, current_time);
        if (myParticles != NULL)
        {
            myParticles->Render(view_matrix, current_time);
        }
    }

    EnemyBulletObject::~EnemyBulletObject()
    {
        delete myParticles;
    }

    bool EnemyBulletObject::handleCollision(glm::vec3 targetPosition, glm::vec3 targetScale)
    {
        if (abs(position_.x - targetPosition.x) < 1.5 * scale_.x * targetScale.x)
        {
            if (abs(position_.y - targetPosition.y) < 1.5 * scale_.y * targetScale.y)
            {
                return true;
            }
        }
        return false;
    }

    void EnemyBulletObject::speedUp(double delta_time)
    {
        velocity_ = velocity_ * glm::vec3(1.005, 1.005, 0);
    }

    bool EnemyBulletObject::checkIsOffScreen()
    {
        if (position_.x < -4 || position_.x > 4)
        {
            return true;
        }
        return false;
    }

} // namespace game