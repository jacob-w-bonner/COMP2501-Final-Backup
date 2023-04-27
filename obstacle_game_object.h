#ifndef OBSTACLE_GAME_OBJECT_H_
#define OBSTACLE_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class ObstacleGameObject : public GameObject {

    public:
        ObstacleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int t);

        // Update function for moving the player object around
        void Update(double delta_time) override;

        // TYPES:
        // Type 0 = Asteroid
        // Type 1 = Comet

    }; // class CollectibleItem

} // namespace game

#endif