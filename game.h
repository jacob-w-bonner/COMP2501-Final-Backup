#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "game_object.h"
#include "bullet_object.h"
#include "drawing_game_object.h"
#include "text_game_object.h"
#include "enemy_handler.h"

namespace game {
    class EnemyHandler;

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void);

            // Getter for an obstacle list
            inline std::vector<GameObject*>& GetObj() { return obstacles_; }
            inline std::vector<BulletObject*>& GetProj() { return projectile_; }

            // Setter for new player particles
            void Hit();

            // Scroll again
            void scrollAgain();

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Sprite geometry
            Geometry *sprite_;

            // Shader for rendering sprites in the scene
            Shader sprite_shader_;

            // Shader for rendering particles
            Shader particle_shader_;

            // Shader for rendering text
            Shader text_shader_;

            // Shader for rendering basic shapes
            Shader drawing_shader_;

            // References to textures
#define NUM_TEXTURES 32
            GLuint tex_[NUM_TEXTURES];

            // List of game objects
            std::vector<GameObject*> game_objects_;
            std::vector<GameObject*> enemies_;
            std::vector<GameObject*> collect_;
            std::vector<GameObject*> obstacles_;
            std::vector<GameObject*> particle_;
            std::vector<DrawingGameObject*> drawing_;
            std::vector<BulletObject*> projectile_;

            // Keep track of time
            double current_time_;

            // Keep track of an explosion disappearance time
            double end_time_;

            // Keep track of invulnerability duration
            double invTime_;

            // Keep track of player lives
            int lives_;

            // Tracks when the gameloop should end
            bool breakout_;

            // Tracks the number of items the player has picked up
            int items_;

            // Tracks if the player is dead
            bool dead;

            // Tracks when to spawn new enemy
            float spawn;

            // Tracks if player is invulnerable or not
            bool invulnerable_;

            //handles enemy spawning/despawning	
            EnemyHandler* enemy_handler;

            // Tracking player health
            int playerHealth;

            // Tracking object spawn rate
            int spawnRate;

            // Scroll factor
            float scrollFac;

            // Scroll threshold
            float threshold;

            // Enemy types
            int enType;

            // Whether or not to spawn enemies
            bool doSpawn;

            // Breakout timer
            double breakoutTime;

            // Boolean for bossfight
            bool boss;

            // Ending the game
            void EndCondition(int);

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void Controls(double delta_time);

            // Update the game based on user input and simulation
            void Update(glm::mat4 view_matrix, double delta_time);

    }; // class Game

} // namespace game

#endif // GAME_H_
