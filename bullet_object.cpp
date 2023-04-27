#include "bullet_object.h"

#include "physical_object.h"
#include "game_object.h"
#include "math_object.h"

#include <math.h>

namespace game {

    /*
        physicalobject inherits from GameObject
        It implements Radius, which is used for collision detection.
    */

    BulletObject::BulletObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float direction, glm::vec3 newSpeed, double ttd)
     : TimedObject(position, geom, shader, texture, ttd)
    {
        angle = direction;
        velocity_ = newSpeed;
        myParticles = NULL;
    }

    void BulletObject::bindParticleSystem(ParticleSystem* newParticles)
    {
        myParticles = newParticles;
    }

    void BulletObject::Update(double delta_time)
    {
        TimedObject::Update(delta_time);
        if (myParticles != NULL)
        {
            myParticles->Update(delta_time);
        }
    }

    void BulletObject::Render(glm::mat4 view_matrix, double current_time)
    {
        GameObject::Render(view_matrix, current_time);
        if (myParticles != NULL)
        {
            myParticles->Render(view_matrix, current_time);
        }
    }

    BulletObject::~BulletObject()
    {
        delete myParticles;
    }

    void BulletObject::speedUp(double delta_time)
    {
        velocity_ = velocity_ * glm::vec3(1.005, 1.005, 0);
    }

} // namespace game