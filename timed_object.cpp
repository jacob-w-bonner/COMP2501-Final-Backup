#include "timed_object.h"
#include "game_object.h"
#include <iostream>

namespace game {

    /*
        timedobject inherits from GameObject
        it implements timetodestroy (ttd), which is used to determine when it will be destroyed.
    */

    TimedObject::TimedObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float ttd)
        : GameObject(position, geom, shader, texture)
    {
        ttd_ = ttd;
        timeAlive = 0;
    }

    double TimedObject::getTTD()
    {
        return ttd_;
    }

    bool TimedObject::isTTD()
    {
        //std::cout << ttd_ << std::endl;
        //std::cout << timeAlive << std::endl;
        if (timeAlive > ttd_)
        {
            return true;
        }
        return false;
    }

    void TimedObject::Update(double delta_time) {
        timeAlive += delta_time;
        // Call the parent's update method to move the object in standard way, if desired
        GameObject::Update(delta_time);
    }

} // namespace game