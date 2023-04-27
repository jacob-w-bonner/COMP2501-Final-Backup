/*#pragma once
#ifndef EN_BEHAVE
#define EN_BEHAVE

#include <glm/glm.hpp>
#include <string>

#include "enemy_game_object.h"

namespace game {

    //enum behaviourType { Circle, Chase, Flee };

    class EnemyBehaviour
    {
        public:
            glm::vec3 getMovement(float, EnemyGameObject*);
            glm::vec3 changeBehaviour(behaviourType);
        protected:
            int behaviour_ID;
            behaviourType behaviour;
    };

}

#endif // !EN_BEHAVE*/