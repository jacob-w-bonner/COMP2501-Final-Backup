#ifndef ENHANDLER
#define ENHANDLER

#include "game.h"
#include "enemy_game_object.h"
#include "player_game_object.h"
#include "bullet_object.h"

namespace game {
    
    enum enemyName {Patroller, LeftMove, RightMove, Targeter, FleeAndShoot, FaceMe, Boss};
    #define TEXTURES 32
    class EnemyGameObject;
    class Game;
    class EnemyHandler
    {
    public:
        EnemyHandler(Geometry*, Shader*, GLuint[TEXTURES], PlayerGameObject* newPlayer, Game* game);
        ~EnemyHandler();
        void init(Geometry*, Shader*, GLuint[TEXTURES], PlayerGameObject* newPlayer, Game* game);
        void createEnemy(enemyName name, float x, float y);
        void UpdateAll(double delta_time);
        void RenderAll(glm::mat4 view_matrix, double current_time);
        void addBulletToList(EnemyBulletObject*);
        Geometry* geom;
        Shader* shader;
        GLuint textures[TEXTURES];
        void handleEnemySpawn();
        bool KillEnemy(int);
        inline GameObject* GetPlayer() { return player; }
        inline std::vector<EnemyGameObject*>& GetEnemies() { return enemyList; }
    private:
        std::vector<EnemyGameObject*> enemyList;
        std::vector<EnemyBulletObject*> masterBulletList;
        PlayerGameObject* player;
        double currentTime;
        int currentCycle;
        Game* g;
    };

}
#endif // !EnemyHandler