#ifndef DRAWING_GAME_OBJECT_H_
#define DRAWING_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class DrawingGameObject : public GameObject {

        public:
            DrawingGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float, int, glm::vec4, GameObject*);

            // Render function for the text
            void Render(glm::mat4 view_matrix, double current_time) override;

            // The value that determines the shader length
            float value;

            // The type
            int type;
            /*
            TYPE == 0: FUEL
            TYPE == 1: HEALTH
            TYPE == 2: VERIFIED
            TYPE == 3: TIMER
            */

            // The colour
            glm::vec4 colour;

            // The parent
            GameObject* parent;

            // Update function for moving the player object around
            void Update(double delta_time) override;

    }; // class DrawingGameObject

} // namespace game

#endif // DRAWING_GAME_OBJECT_H_
