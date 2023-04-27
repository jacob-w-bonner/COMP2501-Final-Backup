#ifndef TIMEDOBJECT_H_
#define TIMEDOBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class TimedObject : public GameObject {

    public:
        TimedObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float ttd);
        // Update function for moving the player object around
        double getTTD();
        bool isTTD();
        void Update(double delta_time);
    protected:
        float ttd_;
        float timeAlive;
    }; // class PhysicalObject

} // namespace game

#endif // TIMEDOBJECT_H_