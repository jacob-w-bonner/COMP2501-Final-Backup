#ifndef PHYSICALOBJECT_H_
#define PHYSICALOBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class PhysicalObject : public GameObject {

        public:
            PhysicalObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float radius);
            // Update function for moving the player object around
            float getRadius();
            glm::vec3 getPosition();
            static glm::vec3 setRandomSpawnLocation(glm::vec3);
        protected: 
            int radius_;
    }; // class PhysicalObject

} // namespace game

#endif // PHYSICALOBJECT_H_