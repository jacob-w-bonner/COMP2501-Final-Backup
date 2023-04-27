#ifndef PARTICLE_SYSTEM_H_
#define PARTICLE_SYSTEM_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class ParticleSystem : public GameObject {

        public:
            ParticleSystem(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GameObject *parent, glm::vec3 particleColour, float, float);

            void Update(double delta_time) override;

            void Render(glm::mat4 view_matrix, double current_time);

        private:
            GameObject *parent_;
            glm::vec3 particleColour;
            float lowAngle;
            float highAngle;

    }; // class ParticleSystem

} // namespace game

#endif // PARTICLE_SYSTEM_H_