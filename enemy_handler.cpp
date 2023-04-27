#include "enemy_handler.h"
#include <stdlib.h>
#include <time.h>
namespace game
{
    EnemyHandler::EnemyHandler(Geometry* newGeom, Shader* newShad, GLuint tex_[TEXTURES], PlayerGameObject* newPlayer, Game* game)
    {
        init(newGeom, newShad, tex_, newPlayer, game);
    }

    EnemyHandler::~EnemyHandler()
    {
        for (EnemyGameObject* i : enemyList)
        {
            delete i;
        }
        for (EnemyBulletObject* i : masterBulletList)
        {
            delete i;
        }
    }

    void EnemyHandler::init(Geometry* newGeom, Shader* newShad, GLuint tex_[TEXTURES], PlayerGameObject* newPlayer, Game* game)
    {
        geom = newGeom;
        shader = newShad;
        for (int i = 0;i < TEXTURES;i++)
        {
            textures[i] = tex_[i];
        }
        player = newPlayer;
        currentTime = 0;
        currentCycle = 0;
        g = game;
    }

    void EnemyHandler::createEnemy(enemyName name, float x, float y)
    {
        // Getting random number
        srand(time(NULL));
        int texVal = rand() % 100 + 1;

        // Getting shooting types
        int shootType = rand() % 100 + 1;
        shotType t;
        if (shootType > 80) {
            t = Tri;
        }
        else {
            t = Single;
        }

        // Determining if enemy is special
        GLuint tweet;
        if (texVal >= 85) {
            tweet = textures[24];
        }

        EnemyGameObject* newEnemy;
        switch (name)
        {
        case Patroller:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 0.5f, Circle, 3, Chase, false, 2, player->GetPosition(), textures[11], None, true);
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[25], 0.5f, Circle, 3, Chase, false, 2, player->GetPosition(), textures[11], None, false);
            }
            break;
        case LeftMove:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 2.0f, MoveLeft, -1, Idle, true, 1.25, player->GetPosition(), textures[11], t, true);
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[1], 2.0f, MoveLeft, -1, Idle, true, 1.25, player->GetPosition(), textures[11], t, false);
            }
            break;
        case RightMove:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 2.0f, MoveRight, -1, Idle, true, 1.25, player->GetPosition(), textures[11], t, true);
                
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[2], 2.0f, MoveRight, -1, Idle, true, 1.25, player->GetPosition(), textures[11], t, false);
            }
            break;
        case Targeter:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 1.5f, Chase, -1, Chase, false, -1, player->GetPosition(), textures[11], None, true);
                
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[27], 1.5f, Chase, -1, Chase, false, -1, player->GetPosition(), textures[11], None, false);
            }
            break;
        case FleeAndShoot:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 1.0f, Circle, 3, Flee, false, 2, player->GetPosition(), textures[11], Tri, true);
                
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[28], 1.0f, Circle, 3, Flee, false, 2, player->GetPosition(), textures[11], Tri, false);
            }
            break;
        case FaceMe:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 1.0f, Idle, 3, Idle, false, 2, player->GetPosition(), textures[11], None, true);
                //std::cout << "GOT HERE" << std::endl;
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[26], 1.0f, Idle, 3, Idle, false, 2, player->GetPosition(), textures[11], None, false);
            }
            break;
        case Boss:
            newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[31], 1.0f, Tes, 3, Tes, false, 2, player->GetPosition(), textures[11], None, false);
            newEnemy->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
            newEnemy->health = 50;
            newEnemy->hit = 0;
            break;
        default:
            if (texVal >= 85) {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, tweet, 2.0f, Idle, -1, Idle, false, -1, player->GetPosition(), textures[11], None, true);
            }
            else {
                newEnemy = new EnemyGameObject(this, glm::vec3(x, y, 0.0f), geom, shader, textures[1], 2.0f, Idle, -1, Idle, false, -1, player->GetPosition(), textures[11], None, false);
            }
            break;
        }
        enemyList.push_back(newEnemy);
    }

    void EnemyHandler::addBulletToList(EnemyBulletObject* newBullet)
    {
        masterBulletList.push_back(newBullet);
    }

    void EnemyHandler::UpdateAll(double delta_time)
    {
        currentTime += delta_time;
        //handleEnemySpawn();

        int bulletIndex = 0;

        for (EnemyBulletObject* thisBullet : masterBulletList)
        {
            //check if the enemy is off screen, delete if so
            if (thisBullet->checkIsOffScreen())
            {
                if (bulletIndex < masterBulletList.size()) {
                    masterBulletList.erase(masterBulletList.begin() + bulletIndex);
                }
                continue;
            }
            else if (thisBullet->isTTD())
            {
                if (bulletIndex < masterBulletList.size()) {
                    masterBulletList.erase(masterBulletList.begin() + bulletIndex);
                }
                continue;
            }
            thisBullet->Update(delta_time);
            if (thisBullet->handleCollision(player->GetPosition(), player->GetScale()))
            {
                if (bulletIndex < masterBulletList.size()) {
                    masterBulletList.erase(masterBulletList.begin() + bulletIndex);
                }
                player->registerCollision();
                g->Hit();
                continue;

            }
            std::vector<GameObject*> objVec = g->GetObj();
            for (int i = 0; i < objVec.size(); i++) {
                GameObject* obObj = objVec[i];
                if (thisBullet->handleCollision(obObj->GetPosition(), obObj->GetScale()))
                {
                    if (bulletIndex < masterBulletList.size()) {
                        masterBulletList.erase(masterBulletList.begin() + bulletIndex);
                    }
                    obObj->health -= 1;
                    obObj->hit += 0.1f;
                    continue;

                }
            }
            bulletIndex++;
        }


        int enemyIndex = 0;
        for (EnemyGameObject* current : enemyList)
        {
            //check if the enemy is off screen, delete if so
            if (current->checkIsOffScreen())
            {

                if (current->getBehaviour() != Tes) {
                    if (enemyIndex < enemyList.size()) {
                        enemyList.erase(enemyList.begin() + enemyIndex);
                    }
                    continue;
                }
            }

            //update the player's position in that enemy object, for chase/flee, then perform the object's update function
            current->setPlayerPos(player->GetPosition());
            current->Update(delta_time);
            //test for collision with the player
            if (current->handleCollision(player->GetPosition(), 0.8f * player->GetScale()))
            {
                if (current->health <= 0) {
                    if (enemyIndex < enemyList.size()) {
                        enemyList.erase(enemyList.begin() + enemyIndex);
                    }
                    player->registerCollision();
                    g->Hit();
                }
                continue;
                
            }
            /*for (BulletObject* playerBullet : player->getBullets())
            {
                if (current->handleCollision(playerBullet->GetPosition(), playerBullet->GetScale()))
                {
                    enemyList.erase(enemyList.begin() + enemyIndex);
                    //Add here: whatever happens when the player shoots an enemy
                    continue;
                }
            }*/
            enemyIndex++;
        }
    }

    bool EnemyHandler::KillEnemy(int index) {
        if (enemyList[index]->getBehaviour() != Tes) {
            if (index < enemyList.size()) {
                enemyList.erase(enemyList.begin() + index);
            }
        }
        else {
            enemyList[index]->health -= 1;
            enemyList[index]->hit += 0.05f;
            if (enemyList[index]->health <= 0) {
                if (index < enemyList.size()) {
                    enemyList.erase(enemyList.begin() + index);
                }
                if (enemyList.size() <= 0) {
                    g->scrollAgain();
                }
                else if (enemyList[index]->getBehaviour() != Tes) {
                    g->scrollAgain();
                }
                return true;
            }
        }
        return false;
    }

    void EnemyHandler::RenderAll(glm::mat4 view_matrix, double current_time)
    {
        for (EnemyGameObject* current : enemyList)
        {
            current->Render(view_matrix, current_time);
        }
        for (EnemyBulletObject* thisBullet : masterBulletList)
        {
            thisBullet->Render(view_matrix, current_time);
        }
    }

    void EnemyHandler::handleEnemySpawn()
    {
        if (currentTime >= 1)
        {
            switch (currentCycle)
            {
            case 0:
                currentCycle++;
                createEnemy(FleeAndShoot, 3, 2);
                createEnemy(LeftMove, 3, 2);
                break;
            case 1:
                currentCycle++;
                createEnemy(RightMove, -3, -2);
                break;
            case 2:
                currentCycle++;
                createEnemy(Targeter, 3, -3);
                break;
            case 3:
                currentCycle++;
                createEnemy(LeftMove, 3, 2);
                break;
            case 4:
                currentCycle++;
                createEnemy(Patroller, -3, 3);
                break;
            case 5:
                currentCycle++;
                createEnemy(FleeAndShoot, 3, 2);
                break;
            case 6:
                currentCycle = 0;
                break;
            }
            currentTime = 0;
        }
    }
}