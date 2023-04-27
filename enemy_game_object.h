#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include <vector>
#include "game_object.h"
#include "enemy_bullet_object.h"
#include "enemy_handler.h"

namespace game {

    enum behaviourType { Idle, Circle, MoveLeft, MoveRight, Chase, Charge, Flee, Tes };
    enum shotType { None, Single, SingleTrack, Tri, Quad, Burst };
    class EnemyHandler;
    // Inherits from GameObject
    class EnemyGameObject : public GameObject {

    public:
        EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);
        EnemyGameObject(EnemyHandler* myNewHandler, const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float newSpeed, behaviourType startBehaviour, float startProximity, behaviourType proxBehaviour, bool doesShoot, float attackSpeed, glm::vec3 playerPosition, GLuint bulletTexture, shotType newType, bool twitter);
        void Initialize();
        // Update function for moving the player object around
        void Update(double delta_time) override;
        // Render function, which now also renders this enemy's bullets
        void Render(glm::mat4 view_matrix, double current_time) override;
        bool checkProximity();

        void handleProximity();
        void handleAttack(float delta_time);
        void shoot();
        bool handleCollision(glm::vec3, glm::vec3);

        void setPlayerPos(glm::vec3 newTarget) { playerPos = newTarget; };
        void setDone(bool d) { done = d; }

        bool checkIsOffScreen();
        // Getter for the rotation position
        inline glm::mat4 GetRotate() { return rotate_; };

        void setBehaviour(behaviourType);
        float getSpeedMultiplier();
        behaviourType getBehaviour() { return currentBehaviour; }
        void rotateBehaviour(int);
        inline bool GetVerif() { return verified; }

    private:
        behaviourType initialBehaviour;
        behaviourType currentBehaviour;
        float proximitySensor; //distance from the player at which this enemy will change behaviour. Set to -1 to disable. 
        float leashRange; //distance at which the enemy will resume its original behaviour
        behaviourType proximityBehaviour;
        glm::vec3 centerPos; //For circle/sin movement, the center of that movement
        glm::vec3 playerPos; //for proximity detection
        float timeAlive; //used for rotations and some other things
        float moveSpeed;
        bool doesAttack; //does the enemy shoot?
        shotType myShotType;
        float attackTimer;
        float attackTimerMax;
        GLuint bulletTexture;
        EnemyHandler* myHandler;
        int verified;
        bool done;

    }; // class EnemyGameObject

} // namespace game

#endif