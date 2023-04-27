#ifndef COLLECTIBLE_ITEM_H_
#define COLLECTIBLE_ITEM_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class CollectibleGameObject : public GameObject {

    public:
        CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int t);

        // Update function for moving the player object around
        void Update(double delta_time) override;

        // TYPES:
        // Type 0 = Fuel Tank
        // Type 1 = Health Pack
        // Type 2 = Twitter Verification

    }; // class CollectibleItem

} // namespace game

#endif