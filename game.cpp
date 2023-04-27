#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <time.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <SOIL/SOIL.h>
#include <iostream>
#include <math.h>

#include <path_config.h>

#include "sprite.h"
#include "shader.h"
#include "player_game_object.h"
#include "enemy_game_object.h"
#include "collectible_game_object.h"
#include "obstacle_game_object.h"
#include "particles.h"
#include "particle_system.h"
#include "bullet_object.h"
#include "drawing_game_object.h"
#include "text_game_object.h"
#include "game.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "OC Transpo: A Space Odyssey";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 1.0);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;

int testVar = 3;

Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set window to not resizable
    // Required or else the calculation to get cursor pos to screenspace will be incorrect
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize particle shader
    particle_shader_.Init((resources_directory_g + std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/particle_fragment_shader.glsl")).c_str());

    // Initialize text shader
    text_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/text_fragment_shader.glsl")).c_str());

    // Initialize drawing shader
    drawing_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/drawing_fragment_shader.glsl")).c_str());

    // Initializing spawn rate
    spawnRate = 50;

}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }
    for (int i = 0; i < enemies_.size(); i++) {
        delete enemies_[i];
    }
    for (int i = 0; i < collect_.size(); i++) {
        delete collect_[i];
    }
    for (int i = 0; i < obstacles_.size(); i++) {
        delete obstacles_[i];
    }
    for (int i = 0; i < projectile_.size(); i++) {
        delete projectile_[i];
    }
    for (int i = 0; i < drawing_.size(); i++) {
        delete drawing_[i];
    }
    for (int i = 0; i < particle_.size(); i++) {
        delete particle_[i];
    }

    delete enemy_handler;

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{
    // Setup the game world

    // Load textures
    SetAllTextures();

    // Setting the number of lives
    lives_ = 2;

    // Setting the loop break condition
    breakout_ = false;

    // Setting the number of items the player has picked up
    items_ = 0;

    // Setting that the user is not invulnerable to start
    invulnerable_ = false;

    // Setting the time for invulnerability
    invTime_ = 0;

    // Determining if the player is dead
    dead = false;

    // Setting up time for new enemy to spawn
    spawn = 7;

    // Setting initial player health
    playerHealth = 10;

    // Setting up random number seed
    srand(time(NULL));

    // Scroll factor
    //scrollFac = 0.01f;
    scrollFac = 0.01f;

    // Enemy types
    enType = 2;

    // Threshold
    threshold = 0.0f;

    // Whether or not to spawn
    doSpawn = true;

    // Setting breakout time
    breakoutTime = 0.0;

    // Setting current time
    current_time_ = 0.0;

    // Setting bossfight flag
    boss = false;

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector
    GameObject* player = new PlayerGameObject(glm::vec3(0.0f, -3.0f, 0.0f), sprite_, &sprite_shader_, tex_[0]);
    player->SetAngle(0.0f);
    player->SetScale(glm::vec3(1.0f, 1.5f, 0.0f));
    game_objects_.push_back(player);
    Geometry* particles_ = new Particles(false);
    particles_->CreateGeometry();
    //std::cout << glm::to_string(player->GetPosition()) << std::endl;

    // Setting up enemy handler
    // Setup other objects
    enemy_handler = new EnemyHandler(sprite_, &sprite_shader_, tex_, (PlayerGameObject*)game_objects_[0], this);
    enemy_handler->createEnemy(Boss, 0.0f, 475.0f);
    //enemy_handler->createEnemy(FaceMe, 2.0f, 0.0f);
    //enemy_handler->createEnemy(Patroller, 2.8f, -1.0f);


    // Particles for the player
    GameObject* particles = new ParticleSystem(glm::vec3(0.0f, -0.65f, 0.0f), particles_, &particle_shader_, tex_[5], player, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
    particles->SetAngle(0.0f);
    particle_.push_back(particles);
    //std::cout << glm::to_string(particles->GetPosition()) << std::endl;

    // Setting gas cannister meter and icon
    DrawingGameObject* drawing = new DrawingGameObject(glm::vec3(-3.73f, -0.75f, -1.0f), sprite_, &drawing_shader_, tex_[12], player->getFuel(), 0, glm::vec4(0.1333, 0.6941, 0.3012, 0.0), player);
    drawing->SetScale(glm::vec3(0.25f, 5.0f, 0.5f));
    drawing_.push_back(drawing);
    GameObject* gascan = new GameObject(glm::vec3(-3.73f, -3.5f, -1.0f), sprite_, &sprite_shader_, tex_[13]);
    gascan->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
    game_objects_.push_back(gascan);

    // Setting the heart and health meter
    DrawingGameObject* healthbar = new DrawingGameObject(glm::vec3(3.73f, -0.75f, -1.0f), sprite_, &drawing_shader_, tex_[15], player->health, 1, glm::vec4(1.0, 0.6392, 0.6941, 0.0), player);
    healthbar->SetScale(glm::vec3(0.25f, 5.0f, 0.5f));
    drawing_.push_back(healthbar);
    GameObject* heart = new GameObject(glm::vec3(3.73f, -3.6f, -1.0f), sprite_, &sprite_shader_, tex_[14]);
    heart->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));
    game_objects_.push_back(heart);

    // Setting the timer and health for the verified powerup
    DrawingGameObject* timer = new DrawingGameObject(glm::vec3(-100.0f, -100.0f, -1.0f), sprite_, &drawing_shader_, tex_[17], player->inv, 3, glm::vec4(1.0, 1.0, 1.0, 0.0), player);
    timer->SetScale(glm::vec3(0.25f, 5.0f, 0.5f));
    drawing_.push_back(timer);
    DrawingGameObject* shield = new DrawingGameObject(glm::vec3(-100.0f, -100.0f, -1.0f), sprite_, &drawing_shader_, tex_[17], player->inv, 2, glm::vec4(0.0, 0.7176, 0.9372, 0.0), player);
    shield->SetScale(glm::vec3(0.25f, 5.0f, 0.5f));
    drawing_.push_back(shield);
    
    // EVERYTHING BEYOND THIS POINT SCROLLS

    // Set up text
    TextGameObject* safe = new TextGameObject(glm::vec3(0.0f, 3.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    safe->SetScale(glm::vec3(5.0, 0.8, 1.0));
    safe->SetText("Safe voyage to Mars!");
    game_objects_.push_back(safe);

    TextGameObject* satellite = new TextGameObject(glm::vec3(0.0f, 2.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    satellite->SetScale(glm::vec3(5.0, 0.8, 1.0));
    satellite->SetText("Watch out for the satellites!");
    game_objects_.push_back(satellite);

    TextGameObject* newsat = new TextGameObject(glm::vec3(0.0f, 103.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    newsat->SetScale(glm::vec3(5.0, 0.8, 1.0));
    newsat->SetText("Two new satellites incoming!");
    game_objects_.push_back(newsat);

    TextGameObject* ast = new TextGameObject(glm::vec3(0.0f, 102.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    ast->SetScale(glm::vec3(5.0, 0.8, 1.0));
    ast->SetText("Wait... are those asteroids?");
    game_objects_.push_back(ast);

    TextGameObject* twomore = new TextGameObject(glm::vec3(0.0f, 303.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    twomore->SetScale(glm::vec3(5.0, 0.8, 1.0));
    twomore->SetText("Two more new satellites!");
    game_objects_.push_back(twomore);

    TextGameObject* crazy = new TextGameObject(glm::vec3(0.0f, 302.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    crazy->SetScale(glm::vec3(5.0, 0.8, 1.0));
    crazy->SetText("This is getting crazy!");
    game_objects_.push_back(crazy);

    TextGameObject* more = new TextGameObject(glm::vec3(0.0f, 453.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    more->SetScale(glm::vec3(5.0, 0.8, 1.0));
    more->SetText("Almost there... wait!");
    game_objects_.push_back(more);

    TextGameObject* first = new TextGameObject(glm::vec3(0.0f, 452.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    first->SetScale(glm::vec3(5.0, 0.8, 1.0));
    first->SetText("We have to get past that!");
    game_objects_.push_back(first);

    TextGameObject* tes = new TextGameObject(glm::vec3(0.0f, 478.0f, 0.0f), sprite_, &text_shader_, tex_[18]);
    tes->SetScale(glm::vec3(5.0, 0.8, 1.0));
    tes->SetText("Destroy the command satellite!");
    game_objects_.push_back(tes);
    
    // Setup other objects
    //enemies_.push_back(new EnemyGameObject(glm::vec3(-2.2f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[2]));
    //enemies_.push_back(new EnemyGameObject(glm::vec3(2.8f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[2], 6));
    //game_objects_.push_back(new CollectibleGameObject(glm::vec3(-3.5f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    //game_objects_.push_back(new CollectibleGameObject(glm::vec3(3.5f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    //collect_.push_back(new CollectibleGameObject(glm::vec3(0.0f, -1.5f, 0.0f), sprite_, &sprite_shader_, tex_[22], 2));
    //game_objects_.push_back(new CollectibleGameObject(glm::vec3(-3.0f, -3.5f, 0.0f), sprite_, &sprite_shader_, tex_[6]));
    //collect_.push_back(new CollectibleGameObject(glm::vec3(0.0f, -1.5f, 0.0f), sprite_, &sprite_shader_, tex_[6], 1));
    //obstacles_.push_back(new ObstacleGameObject(glm::vec3(0.0f, 5.0f, 0.0f), sprite_, &sprite_shader_, tex_[7], 0));

    // Planet sprites
    GameObject* earth = new GameObject(glm::vec3(0.0f, -4.0f, 0.0f), sprite_, &sprite_shader_, tex_[29]);
    earth->SetScale(glm::vec3(5.0f, 5.0f, 10.0f));
    game_objects_.push_back(earth);
    GameObject* mars = new GameObject(glm::vec3(0.0f, 500.0f, 0.0f), sprite_, &sprite_shader_, tex_[30]);
    mars->SetScale(glm::vec3(5.0f, 5.0f, 10.0f));
    game_objects_.push_back(mars);

    //Set up the boss
    //theBoss = new EnemyGameObject(enemy_handler, glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[31], 0.5f, Circle, 3, Idle, false, 2, player->GetPosition(), tex_[11], None, false);
    //theBoss->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    //theBoss->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

    // Setup background
    // In this specific implementation, the background is always the
    // last objects
    // Starting background
    GameObject *background1 = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[8]);
    background1->SetScale(glm::vec3(200.0f, 200.0f, 1.0f));
    background1->tileNum = 100;
    game_objects_.push_back(background1);

    // Second background
    GameObject* background2 = new GameObject(glm::vec3(0.0f, 198.0f, 0.0f), sprite_, &sprite_shader_, tex_[9]);
    background2->SetScale(glm::vec3(200.0f, 200.0f, 1.0f));
    background2->tileNum = 100;
    game_objects_.push_back(background2);

    // Third background
    GameObject* background3 = new GameObject(glm::vec3(0.0f, 350.0f, 0.0f), sprite_, &sprite_shader_, tex_[10]);
    background3->SetScale(glm::vec3(200.0f, 200.0f, 1.0f));
    background3->tileNum = 100;
    game_objects_.push_back(background3);

    // Fourth background
    GameObject* background4 = new GameObject(glm::vec3(0.0f, 500.0f, 0.0f), sprite_, &sprite_shader_, tex_[19]);
    background4->SetScale(glm::vec3(200.0f, 200.0f, 1.0f));
    background4->tileNum = 100;
    game_objects_.push_back(background4);
    
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    glGenTextures(NUM_TEXTURES, tex_);
    SetTexture(tex_[0], (resources_directory_g+std::string("/textures/bus.png")).c_str());
    SetTexture(tex_[1], (resources_directory_g+std::string("/textures/satellite3.png")).c_str());
    SetTexture(tex_[2], (resources_directory_g+std::string("/textures/satellite4.png")).c_str());
    SetTexture(tex_[3], (resources_directory_g+std::string("/textures/stars.png")).c_str());
    SetTexture(tex_[4], (resources_directory_g+std::string("/textures/orb.png")).c_str());
    SetTexture(tex_[5], (resources_directory_g+std::string("/textures/explosion.png")).c_str());
    SetTexture(tex_[6], (resources_directory_g+std::string("/textures/tire.png")).c_str());
    SetTexture(tex_[7], (resources_directory_g+std::string("/textures/Asteroid.png")).c_str());
    SetTexture(tex_[8], (resources_directory_g+std::string("/textures/Space_Stars1.png")).c_str());
    SetTexture(tex_[9], (resources_directory_g+std::string("/textures/Space_Stars5.png")).c_str());
    SetTexture(tex_[10], (resources_directory_g+std::string("/textures/Space_Stars3.png")).c_str());
    SetTexture(tex_[11], (resources_directory_g+std::string("/textures/enbul.png")).c_str());
    SetTexture(tex_[12], (resources_directory_g+std::string("/textures/darkgreen.png")).c_str());
    SetTexture(tex_[13], (resources_directory_g+std::string("/textures/gas.png")).c_str());
    SetTexture(tex_[14], (resources_directory_g+std::string("/textures/heart.png")).c_str());
    SetTexture(tex_[15], (resources_directory_g+std::string("/textures/red.png")).c_str());
    SetTexture(tex_[16], (resources_directory_g+std::string("/textures/lightblue.png")).c_str());
    SetTexture(tex_[17], (resources_directory_g+std::string("/textures/darkblue.png")).c_str());
    SetTexture(tex_[18], (resources_directory_g+std::string("/textures/font.png")).c_str());
    SetTexture(tex_[19], (resources_directory_g+std::string("/textures/Space_Stars7.png")).c_str());
    SetTexture(tex_[20], (resources_directory_g+std::string("/textures/heartcollect.png")).c_str());
    SetTexture(tex_[21], (resources_directory_g+std::string("/textures/gascollect.png")).c_str());
    SetTexture(tex_[22], (resources_directory_g+std::string("/textures/verified.png")).c_str());
    SetTexture(tex_[23], (resources_directory_g+std::string("/textures/playbul.png")).c_str());
    SetTexture(tex_[24], (resources_directory_g+std::string("/textures/satellite7.png")).c_str());
    SetTexture(tex_[25], (resources_directory_g+std::string("/textures/satellite1.png")).c_str());
    SetTexture(tex_[26], (resources_directory_g+std::string("/textures/satellite2.png")).c_str());
    SetTexture(tex_[27], (resources_directory_g+std::string("/textures/satellite5.png")).c_str());
    SetTexture(tex_[28], (resources_directory_g+std::string("/textures/satellite6.png")).c_str());
    SetTexture(tex_[29], (resources_directory_g+std::string("/textures/earth.png")).c_str());
    SetTexture(tex_[30], (resources_directory_g+std::string("/textures/mars.png")).c_str());
    SetTexture(tex_[31], (resources_directory_g+std::string("/textures/head.png")).c_str());
    glBindTexture(GL_TEXTURE_2D, tex_[0]);

}


void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Clear background
        glClearColor(viewport_background_color_g.r,
                     viewport_background_color_g.g,
                     viewport_background_color_g.b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view to zoom out, centered by default at 0,0
        float camera_zoom = 0.25f;
        glm::mat4 view_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update other events like input handling
        glfwPollEvents();

        // Update the game
        Update(view_matrix, delta_time);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Condition to end the game
        if (breakout_) {
            break;
        }
    }
}

void Game::scrollAgain() {
    // Explosion
    Geometry* particles_ = new Particles(true);
    particles_->CreateGeometry();
    GameObject* particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
    particles->SetAngle(0.0f);
    particles->despawn = current_time_ + 1.0;
    particles->SetVelocity(glm::vec3(0.0f, -0.1f, 0.0f));
    particle_.push_back(particles);

    // Scrolling again 
    scrollFac = 0.01f;
}


void Game::EndCondition(int type) {

    // Stop level progressing
    scrollFac = 0;

    // Stopping enemies
    std::vector<EnemyGameObject*> enVec = enemy_handler->GetEnemies();
    for (int i = 0; i < enVec.size(); i++) {
        enVec[i]->setDone(true);
        //enVec[i]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // Stopping collectible movement
    for (int i = 0; i < collect_.size(); i++) {
        collect_[i]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // Stop obstacle movement
    for (int i = 0; i < obstacles_.size(); i++) {
        obstacles_[i]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // Out of fuel
    if (type == 0) {
        // Setting a closing time
        if (breakoutTime <= 0) {
            breakoutTime = current_time_ + 6.0f;

            // Adding a game over text
            TextGameObject* fuel = new TextGameObject(glm::vec3(0.0f, 0.0f, -2.0f), sprite_, &text_shader_, tex_[18]);
            fuel->SetScale(glm::vec3(5.0, 0.8, 1.0));
            fuel->SetText("Out of fuel. Game Over!");
            game_objects_.push_back(fuel);

            // Removing particles if possible
            if (particle_.size() > 0) {
                particle_.erase(particle_.begin());
            }
        }

        // Reducing player velocity
        if (game_objects_[0]->GetVelocity()[1] > 0.0f) {
            game_objects_[0]->SetVelocity(game_objects_[0]->GetVelocity() + glm::vec3(0.0f, -0.025f, 0.0f));
        }
        if (game_objects_[0]->GetVelocity()[0] > 0.0f) {
            game_objects_[0]->SetVelocity(game_objects_[0]->GetVelocity() + glm::vec3(-0.025f, 0.0f, 0.0f));
        }
    }

    // Dead
    if (type == 1) {
        // Setting a closing time
        if (breakoutTime <= 0) {
            breakoutTime = current_time_ + 4.0f;

            // Adding a game over text
            TextGameObject* death = new TextGameObject(glm::vec3(0.0f, 0.0f, -2.0f), sprite_, &text_shader_, tex_[18]);
            death->SetScale(glm::vec3(5.0, 0.8, 1.0));
            death->SetText("You died. Game Over!");
            game_objects_.push_back(death);

            // Adding death particles
            Geometry* particles_ = new Particles(true);
            particles_->CreateGeometry();
            GameObject* particles = new ParticleSystem(game_objects_[0]->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
            particles->SetAngle(0.0f);
            particles->despawn = current_time_ + 2.0;
            particle_.push_back(particles);

            // Removing player from view (not deleting for segmentation fault reasons)
            game_objects_[0]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
            game_objects_[0]->SetPosition(glm::vec3(1000.0f, 1000.0f, 1000.0f));
            game_objects_[0]->setFuel(1000.0f);
        }
    }

    // Reached end
    if (type == 2) {
        if (breakoutTime <= 0) {
            breakoutTime = current_time_ + 6.0f;

            // Adding a game over text
            TextGameObject* yay = new TextGameObject(glm::vec3(0.0f, -2.5f, -2.0f), sprite_, &text_shader_, tex_[18]);
            yay->SetScale(glm::vec3(5.0, 0.8, 1.0));
            yay->SetText("You made it to Mars!");
            game_objects_.push_back(yay);

            // Moving player towards mars
            glm::vec3 targetVector = glm::vec3(0.0f, 4.0f, 0.0f) - game_objects_[0]->GetPosition();
            glm::vec3 normalizedTargetVector = targetVector / glm::length(targetVector);
            glm::vec3 velocityVector = normalizedTargetVector;
            game_objects_[0]->SetVelocity(velocityVector);
            //game_objects_[0]->SetVelocity(targetVector);
        }
    }

    // Breaking out of the game
    if (current_time_ > breakoutTime) {
        breakout_ = true;
    }
}

void Game::Hit() {
    Geometry* particles_ = new Particles(true);
    particles_->CreateGeometry();
    GameObject* particles = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[5], game_objects_[0], glm::vec3(10.0f, 0.0f, 0.0f), -180.0f, 180.0f);
    particles->SetAngle(0.0f);
    particles->SetScale(glm::vec3(0.5f, 0.5f, 0.0f));
    particles->despawn = current_time_ + 0.5;
    particle_.push_back(particles);
}

void Game::Update(glm::mat4 view_matrix, double delta_time)
{

    // Update time
    current_time_ += delta_time;

    // Updating threshold
    threshold += scrollFac;

    // Handle user input
    if (lives_ >= 0) {
        if (breakoutTime <= 0.0) {
            Controls(delta_time);
        }
        for (int i = game_objects_.size() - 15; i < game_objects_.size(); i++) {
            game_objects_[i]->SetPosition(game_objects_[i]->GetPosition() - glm::vec3(0.0f, scrollFac, 0.0f));
        }
    }

    // Checking to see if scrolling should be stopped
    if (threshold >= 475.0f && boss == false) {
        scrollFac = 0.0f;
        threshold = 475.0f;
        boss = true;
        doSpawn = true;
    }

    // Checking to see if new enemy should spawn
    if (current_time_ > spawn && doSpawn == true && breakoutTime <= 0.0) {
        //std::cout << threshold << std::endl;
        // Choosing the kind of enemy to spawn
        int type = rand() % enType;
       
        // Spawn coordinates if necessary
        int xCoord;
        int yCoord;

        // Spawning patrolling enemy
        switch (type) {
        // Spawning patroller enemy
        case 0:
            xCoord = rand() % 7 + (-3);
            enemy_handler->createEnemy(Patroller, xCoord, 5.0f);
            break;
        // Spawning a stationary enemy
        case 1:
            xCoord = rand() % 7 + (-3);
            enemy_handler->createEnemy(FaceMe, xCoord, 5.0f);
            break;
        // Spawning an enemy that moves right to left
        case 2:
            yCoord = rand() % 2 + 2;
            enemy_handler->createEnemy(RightMove, -5.0f, yCoord);
            break;
        // Spawning an enemy that moves left to right
        case 3:
            yCoord = rand() % 2 + 2;
            enemy_handler->createEnemy(LeftMove, 5.0f, yCoord);
            break;
        // Spawning an enemy that targets the player
        case 4:
            xCoord = rand() % 7 + (-3);
            enemy_handler->createEnemy(Targeter, xCoord, 5.0f);
            break;
        // Spawning an enemy that flees and shoots at the player
        case 5:
            xCoord = rand() % 7 + (-3);
            enemy_handler->createEnemy(FleeAndShoot, xCoord, 5.0f);
            break;
        // Default enemy is patrolling enemy
        default:
            xCoord = rand() % 7 + (-3);
            //enemy_handler->createEnemy(Patroller, xCoord, 5.0f);
            break;
        }

        // Adjusting the spawn timer
        if (threshold > 105.0f && threshold < 305.0f) {
            spawn += 1.5f;
        }
        else if (threshold > 305.0f && threshold < 450.0f) {
            spawn += 1;
        }
        else if (threshold > 450.0f) {
            //std::cout << "GOT HERE" << std::endl;
            //doSpawn = false;
            spawn += 1.5;
        }
        else {
            spawn += 2;
        }

        // Updating what enemies can spawn
        if (threshold > 105.0f && enType == 2) {
            enType += 2;
        }
        else if (threshold > 305.0f && enType == 4) {
            enType += 2;
        }

        // Getting random number for asteroid spawning
        int astType = rand() % 100 + 1;

        // Spawning asteroid if necessary
        if (astType > 85 && threshold > 105.0f && threshold < 450.0f) {
            xCoord = rand() % 7 + (-3);
            obstacles_.push_back(new ObstacleGameObject(glm::vec3(xCoord, 5.0f, 0.0f), sprite_, &sprite_shader_, tex_[7], 0));
        }

        //obstacles_.push_back(new ObstacleGameObject(glm::vec3(0.0f, 5.0f, 0.0f), sprite_, &sprite_shader_, tex_[7], 0));
        //collect_.push_back(new CollectibleGameObject(glm::vec3(xCoord, yCoord, 0.0f), sprite_, &sprite_shader_, tex_[4], 0));
        //collect_.push_back(new CollectibleGameObject(glm::vec3(xCoord, yCoord, 0.0f), sprite_, &sprite_shader_, tex_[4], 1));
        //collect_.push_back(new CollectibleGameObject(glm::vec3(xCoord, yCoord, 0.0f), sprite_, &sprite_shader_, tex_[4], 2));
    }

    // Updating enemy handler
    enemy_handler->UpdateAll(delta_time);
    enemy_handler->RenderAll(view_matrix, current_time_);

    // Update and render all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        // Updating the player's current position in other objects
        current_game_object->player = game_objects_[0]->GetPosition();

        // Update the current game object
        current_game_object->Update(delta_time);

        // Checking particles
        for (int j = 0; j < particle_.size(); j++) {
            
            // Grabbing the particles from the vector
            GameObject* parObj = particle_[j];

            // Update the current game object
            parObj->Update(delta_time);

            // Rendering the particle system object
            parObj->Render(view_matrix, current_time_);

            // Checking to see if particle should be deleted
            if (parObj->despawn > 0.0 && current_time_ > parObj->despawn) {
                if (j < particle_.size()) {
                    particle_.erase(particle_.begin() + j);
                }
            }

        }

        // Checking obstacle objects
        for (int j = 0; j < obstacles_.size(); j++) {

            // Pulling object from vector
            GameObject* obObj = obstacles_[j];

            // Checking to see if asteroid has 0 health
            if (obObj->health <= 0) {
                if (j < obstacles_.size()) {
                    obstacles_.erase(obstacles_.begin() + j);
                }
                continue;
            }

            // Updating the obstacle
            obObj->Update(delta_time);
            obObj->Render(view_matrix, delta_time);

            // Handling asteroid behaviour
            if (obObj->getType() == 0) {
                // Pulling the player from the game object list
                GameObject* player = game_objects_[0];

                // Checking for collision between the player and the object
                // Compute distance between object i and object j
                float distance = glm::length(player->GetPosition() - obObj->GetPosition());

                // If distance is below a lower threshold, we have a collision
                if (distance < player->GetScale()[0] - 0.2f && player->collision == false && !player->getVerified()) {

                    // Changing the collision boolean to be true and subtracting player health
                    player->health -= 1;
                    player->hit += 0.05f;
                    player->collision = true;
                    Hit();

                    // Normalizing location
                    glm::vec3 normalized = glm::normalize(player->GetPosition() - obObj->GetPosition());

                    // Calculating the new velocities of the objects
                    glm::vec3 playerVel = player->GetVelocity() - ((float)((2 * obObj->GetMass()) / (player->GetMass() + obObj->GetMass()))) * glm::dot(normalized, (player->GetVelocity() - obObj->GetVelocity())) * normalized;
                    glm::vec3 obVel = obObj->GetVelocity() - ((float)((2 * player->GetMass()) / (player->GetMass() + obObj->GetMass()))) * glm::dot(normalized, (obObj->GetVelocity() - player->GetVelocity())) * normalized;
                    //std::cout << glm::to_string(2.0f * playerVel) << std::endl;
                    //std::cout << glm::to_string(0.5f * obVel) << std::endl;

                    // Setting the new velocities and using multipliers to make collision more satisfying
                    player->SetVelocity(1.0f * playerVel);
                    obObj->SetVelocity(0.1f * obVel);

                } else if (distance < player->GetScale()[0] - 0.2f && player->collision == false && player->getVerified()) {

                    // Destroying asteroid and subtracting shield
                    player->shield -= 1;
                    player->collision = true;
                    Hit();

                    // Particles for the explosion
                    Geometry* particles_ = new Particles(true);
                    particles_->CreateGeometry();
                    GameObject* particles = new ParticleSystem(obObj->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
                    particles->SetAngle(0.0f);
                    particles->despawn = current_time_ + 1.0;
                    particle_.push_back(particles);
                    if (j < obstacles_.size()) {
                        obstacles_.erase(obstacles_.begin() + j);
                    }
                    continue;

                } else if (distance > player->GetScale()[0] + 0.1 && player->collision == true) {
                    // Changing that the collision is false
                    player->collision = false;
                }
            }

            // Checking enemies
            std::vector<EnemyGameObject*> enVec = enemy_handler->GetEnemies();

            for (int k = 0; k < enVec.size(); k++) {

                // Grabbing the object
                EnemyGameObject* enObj = enVec[k];
                //std::cout << enObj->GetPosition()[1] << std::endl;

                // Checking to see if the bullet struck the enemy
                if (abs(obObj->GetPosition().x - enObj->GetPosition().x) < 0.5 * obObj->GetScale().x * enObj->GetScale().x)
                {
                    if (abs(obObj->GetPosition().y - enObj->GetPosition().y) < 0.5 * obObj->GetScale().y * enObj->GetScale().y)
                    {

                        // Killing enemy
                        Geometry* particles_ = new Particles(true);
                        particles_->CreateGeometry();
                        GameObject* particles = new ParticleSystem(enObj->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
                        particles->SetAngle(0.0f);
                        particles->SetScale(glm::vec3(0.5f, 0.5f, 0.0f));
                        particles->despawn = current_time_ + 0.2;
                        particle_.push_back(particles);
                        enemy_handler->KillEnemy(k);
                    }
                }
            }

            // Checking for bounds to delete object
            if (obObj->GetPosition()[0] < -6.0f || obObj->GetPosition()[0] > 6.0f ||
                obObj->GetPosition()[1] < -6.0f || obObj->GetPosition()[1] > 6.0f) {
                if (j < obstacles_.size()) {
                    obstacles_.erase(obstacles_.begin() + j);
                }
            }
        }

        // Check for collision with other game objects
        // Note the loop bounds: we avoid testing the last object since
        // it's the background covering the whole game world
        for (int j = 0; j < collect_.size(); j++) {
            GameObject* colObj = collect_[j];

            // Checking fuel packs
            if (colObj->getType() == 0) {
                // Compute distance between object i and object j
                float distance = glm::length(current_game_object->GetPosition() - colObj->GetPosition());

                // If distance is below a lower threshold, we have a collision
                if (distance < current_game_object->GetScale()[0] - 0.2f && current_game_object->getFuel() > 0) {

                    // Adding fuel to the object
                    float fuel = current_game_object->getFuel() + 2.0f;
                    if (fuel > 10.0f) {
                        fuel = 10.0f;
                    }
                    current_game_object->setFuel(fuel);

                    // Removing fuel cannister
                    if (j < collect_.size()) {
                        collect_.erase(collect_.begin() + j);
                    }

                }
            }

            // Checking health packs
            if (colObj->getType() == 1) {
                // Compute distance between object i and object j
                float distance = glm::length(current_game_object->GetPosition() - colObj->GetPosition());

                // If distance is below a lower threshold, we have a collision
                if (distance < current_game_object->GetScale()[0] - 0.2f && current_game_object->health > 0) {

                    // Adding health
                    if (current_game_object->health < 10) {
                        current_game_object->health += 1;
                        current_game_object->hit -= 0.05f;
                    }

                    // Removing health pack
                    if (j < collect_.size()) {
                        collect_.erase(collect_.begin() + j);
                    }

                }
            }

            // Checking verified power ups
            if (colObj->getType() == 2 && current_game_object == game_objects_[0]) {

                // Compute distance between object i and object j
                float distance = glm::length(current_game_object->GetPosition() - colObj->GetPosition());

                // If distance is below a lower threshold, we have a collision
                if (distance < current_game_object->GetScale()[0] - 0.2f && current_game_object->health > 0) {

                    // Verifying object
                    current_game_object->setVerified(true);
                    current_game_object->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    SetTexture(tex_[0], (resources_directory_g + std::string("/textures/tweetyship.png")).c_str());
                    current_game_object->shield = 10;
                    current_game_object->hit = 0.0f;
                    invTime_ = current_time_ + 10;
                    current_game_object->inv = invTime_;
                    particle_[0]->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));

                    // Removing verified object
                    if (j < collect_.size()) {
                        collect_.erase(collect_.begin() + j);
                    }

                }
            }

            colObj->Update(delta_time);
            colObj->Render(view_matrix, delta_time);

            // Checking if the object is off screen
            if (colObj->GetPosition()[1] < -7.0f) {
                if (j < collect_.size()) {
                    collect_.erase(collect_.begin() + j);
                }
            }
        }

        // Rendering fired projectiles
        for (int j = 0; j < projectile_.size(); j++) {
            // Grabbing the projectile object
            BulletObject* bulObj = projectile_[j];

            // Rendering the object
            bulObj->Update(delta_time);
            bulObj->Render(view_matrix, delta_time);

            // Checking for deletion of the projectile

            if (bulObj->isTTD()) {
                glm::vec3 tempPos = bulObj->GetPosition();
                bool ex = bulObj->explode;
                if (j < projectile_.size()) {
                    projectile_.erase(projectile_.begin() + j);
                }
                //std::cout << "GOT HERE" << std::endl;
                //std::cout << current_time_ << std::endl;

                // Particles for the explosion
                if (ex) {
                    Geometry* particles_ = new Particles(true);
                    particles_->CreateGeometry();
                    GameObject* particles = new ParticleSystem(tempPos, particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 10.0f, 0.0f), -180.0f, 180.0f);
                    particles->SetAngle(0.0f);
                    particles->despawn = current_time_ + 0.2;
                    particle_.push_back(particles);
                }

                // Checking to see if object explodes
                if (ex) {

                    // Checking to see if the explosion catches obstacle objects
                    for (int k = 0; k < obstacles_.size(); k++) {

                        // Grabbing the object
                        GameObject* obObj = obstacles_[k];

                        // Compute distance between object j and object k
                        float distance = glm::length(tempPos - obObj->GetPosition());

                        // If distance is below a lower threshold, we have a collision
                        if (distance < 2.0f) {

                            // Subtracting health from the struck object
                            obObj->health -= 5;

                            // Checking to see if the struck object should be destroyed
                            if (obObj->health == 0) {
                                // Particles for the explosion
                                Geometry* particles_ = new Particles(true);
                                particles_->CreateGeometry();
                                GameObject* particles = new ParticleSystem(obObj->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
                                particles->SetAngle(0.0f);
                                particles->despawn = current_time_ + 1.0;
                                particle_.push_back(particles);
                                if (k < obstacles_.size()) {
                                    obstacles_.erase(obstacles_.begin() + k);
                                }
                            }

                        }

                    }

                    // Checking enemies
                    std::vector<EnemyGameObject*> enVec = enemy_handler->GetEnemies();

                    for (int k = 0; k < enVec.size(); k++) {

                        // Grabbing the object
                        EnemyGameObject* enObj = enVec[k];

                        // Checking to see if the bullet struck the enemy
                        if (abs(bulObj->GetPosition().x - enObj->GetPosition().x) < 2 * bulObj->GetScale().x * enObj->GetScale().x)
                        {
                            if (abs(bulObj->GetPosition().y - enObj->GetPosition().y) < 2 * bulObj->GetScale().y * enObj->GetScale().y)
                            {
                                // Spawning loot
                                if (enObj->GetVerif() && enObj->health <= 0) {
                                    // Spawning verified powerup
                                    CollectibleGameObject* coll = new CollectibleGameObject(enObj->GetPosition(), sprite_, &sprite_shader_, tex_[22], 2);
                                    coll->SetVelocity(glm::vec3(0.0f, -0.2f, 0.0f));
                                    collect_.push_back(coll);
                                }
                                else if (enObj->health <= 0) {

                                    // Getting a random number
                                    srand(time(NULL));
                                    int bonus = rand() % 100 + 1;

                                    // Checking for fuel powerup
                                    if (bonus > spawnRate && bonus <= 75) {
                                        CollectibleGameObject* coll = new CollectibleGameObject(enObj->GetPosition(), sprite_, &sprite_shader_, tex_[21], 0);
                                        coll->SetVelocity(glm::vec3(0.0f, -0.2f, 0.0f));
                                        collect_.push_back(coll);

                                        // Checking for health powerup
                                    }
                                    else if (bonus > 75) {
                                        CollectibleGameObject* coll = new CollectibleGameObject(enObj->GetPosition(), sprite_, &sprite_shader_, tex_[20], 1);
                                        coll->SetVelocity(glm::vec3(0.0f, -0.2f, 0.0f));
                                        coll->SetScale(glm::vec3(1.5f, 1.5f, 0.0f));
                                        collect_.push_back(coll);
                                    }
                                }

                                // Killing enemy
                                Geometry* particles_ = new Particles(true);
                                particles_->CreateGeometry();
                                GameObject* particles = new ParticleSystem(enObj->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
                                particles->SetAngle(0.0f);
                                particles->SetScale(glm::vec3(0.5f, 0.5f, 0.0f));
                                particles->despawn = current_time_ + 0.2;
                                particle_.push_back(particles);
                                enemy_handler->KillEnemy(k);

                                
                            }
                        }
                    }

                }

                // Continuing the loop
                continue;

                // Checking if the boss object
                
            }

            // Rotating the projectiles if needed
            //if (!game_objects_[0]->getVerified()) {
                //bulObj->SetRotate(glm::rotate(bulObj->GetRotate(), glm::radians(2.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
           // }

            // Checking for collision with enemies
            std::vector<EnemyGameObject*> enVec = enemy_handler->GetEnemies();
            bool col = false;
            for (int k = 0; k < enVec.size(); k++) {
                
                // Grabbing the object
                EnemyGameObject* enObj = enVec[k];

                // Checking to see if the bullet struck the enemy
                if (abs(bulObj->GetPosition().x - enObj->GetPosition().x) < 1.5 * bulObj->GetScale().x * enObj->GetScale().x)
                {
                    if (abs(bulObj->GetPosition().y - enObj->GetPosition().y) < 1.5 * bulObj->GetScale().y * enObj->GetScale().y && !bulObj->explode)
                    {
                        // Spawning loot
                        if (enObj->GetVerif() && enObj->health <= 0) {
                            // Spawning verified powerup
                            CollectibleGameObject* coll = new CollectibleGameObject(enObj->GetPosition(), sprite_, &sprite_shader_, tex_[22], 2);
                            coll->SetVelocity(glm::vec3(0.0f, -0.2f, 0.0f));
                            collect_.push_back(coll);
                        }
                        else if (enObj->health <= 0) {
                            
                            // Getting a random number
                            srand(time(NULL));
                            int bonus = rand() % 100 + 1;

                            // Checking for fuel powerup
                            if (bonus > spawnRate && bonus <= 75) {
                                CollectibleGameObject* coll = new CollectibleGameObject(enObj->GetPosition(), sprite_, &sprite_shader_, tex_[21], 0);
                                coll->SetVelocity(glm::vec3(0.0f, -0.2f, 0.0f));
                                collect_.push_back(coll);

                            // Checking for health powerup
                            } else if (bonus > 75) {
                                CollectibleGameObject* coll = new CollectibleGameObject(enObj->GetPosition(), sprite_, &sprite_shader_, tex_[20], 1);
                                coll->SetVelocity(glm::vec3(0.0f, -0.2f, 0.0f));
                                coll->SetScale(glm::vec3(1.5f, 1.5f, 0.0f));
                                collect_.push_back(coll);
                            }
                        }

                        // Killing enemy
                        if (j < projectile_.size()) {
                            projectile_.erase(projectile_.begin() + j);
                        }
                        Geometry* particles_ = new Particles(true);
                        particles_->CreateGeometry();
                        GameObject* particles = new ParticleSystem(enObj->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
                        particles->SetAngle(0.0f);
                        particles->SetScale(glm::vec3(0.5f, 0.5f, 0.0f));
                        particles->despawn = current_time_ + 0.2;
                        particle_.push_back(particles);
                        enemy_handler->KillEnemy(k);
                        col = true;

                        
                    }
                }
            }

            // Avoiding segmentation faults
            if (col) {
                continue;
            }

            // Checking for collision with obstacles
            for (int k = 0; k < obstacles_.size(); k++) {

                // Grabbing the object
                GameObject* obObj = obstacles_[k];

                // Compute distance between object j and object k
                float distance = glm::length(bulObj->GetPosition() - obObj->GetPosition());

                // If distance is below a lower threshold, we have a collision
                if (distance < 3 * bulObj->GetScale()[0] - 0.2f && !bulObj->explode) {

                    // Deleting bullet object
                    if (j < projectile_.size()) {
                        projectile_.erase(projectile_.begin() + j);
                    }

                    // Subtracting health from the struck object
                    obObj->health -= 1;
                    obObj->hit += 0.1f;
                    //std::cout << obObj->health << std::endl;

                    // Checking to see if the struck object should be destroyed
                    if (obObj->health == 0) {
                        // Particles for the explosion
                        Geometry* particles_ = new Particles(true);
                        particles_->CreateGeometry();
                        GameObject* particles = new ParticleSystem(obObj->GetPosition(), particles_, &particle_shader_, tex_[5], NULL, glm::vec3(10.0f, 5.0f, 0.0f), -180.0f, 180.0f);
                        particles->SetAngle(0.0f);
                        particles->despawn = current_time_ + 1.0;
                        particle_.push_back(particles);
                        if (k < obstacles_.size()) {
                            obstacles_.erase(obstacles_.begin() + k);
                        }
                    }

                }

            }

        }

        // Resetting the explosion at the proper time
        if (current_time_ >= end_time_ && end_time_ > 0) {
            game_objects_[game_objects_.size() - 3]->SetPosition(glm::vec3(100.0f, 100.0f, 100.0f));
            end_time_ = 0;

            // Ending the game upon player death
            if (lives_ < 0) {
                std::cout << "Game Over" << std::endl;
                breakout_ = true;
            }
        }

        // Reseting the player at the proper time
        if ((current_time_ >= invTime_ && invTime_ > 0 || game_objects_[0]->shield == 0) && game_objects_[0]->getVerified()) {
            SetTexture(tex_[0], (resources_directory_g + std::string("/textures/bus.png")).c_str());
            game_objects_[0]->setVerified(false);
            game_objects_[0]->shield = 0;
            game_objects_[0]->hit = game_objects_[0]->hit = 0.05f * (10.0f - game_objects_[0]->health);
            invTime_ = 0;
            game_objects_[0]->inv = invTime_ - current_time_;
            particle_[0]->SetPosition(glm::vec3(0.0f, -0.65f, 0.0f));
        }

        // Updating health bars
        for (int j = 0; j < drawing_.size(); j++) {
            // Grabbing object
            DrawingGameObject* drawObj = drawing_[j];

            // Rendering object
            drawObj->Update(delta_time);
            drawObj->Render(view_matrix, delta_time);

            // Checking if fuel and health bar should be rendered
            if ((drawObj->type == 0 || drawObj->type == 1) && drawObj->parent->getVerified()) {
                drawObj->SetPosition(glm::vec3(-100.0f, -100.0f, 0.0f));
            } else if ((drawObj->type == 0 || drawObj->type == 1) && !drawObj->parent->getVerified()) {
                if (drawObj->type == 0) {
                    drawObj->SetPosition(glm::vec3(-3.73f, -0.75f, 0.0f));
                    //std::cout << "0" << std::endl;
                } else if (drawObj->type == 1) {
                    drawObj->SetPosition(glm::vec3(3.73f, -0.75f, 0.0f));
                    //std::cout << "1" << std::endl;
                }
            }

            // Checking if timer and shield bars should be rendered
            if ((drawObj->type == 2 || drawObj->type == 3) && drawObj->parent->getVerified()) {
                if (drawObj->type == 3) {
                    drawObj->SetPosition(glm::vec3(-3.73f, -0.75f, 0.0f));
                }
                else if (drawObj->type == 2) {
                    drawObj->SetPosition(glm::vec3(3.73f, -0.75f, 0.0f));
                }
            }
            else if ((drawObj->type == 2 || drawObj->type == 3) && !drawObj->parent->getVerified()) {
                drawObj->SetPosition(glm::vec3(-100.0f, -100.0f, 0.0f));
            }
        }

        // Setting the fuel and time to the drawings
        current_game_object->setFuel(current_game_object->getFuel());
        current_game_object->inv = invTime_ - current_time_;

        // Render game object
        current_game_object->Render(view_matrix, current_time_);
    }

    // Checking to see if the game should end
    if (game_objects_[0]->getFuel() <= 0) {
        EndCondition(0);
    }
    else if (game_objects_[0]->health <= 0) {
        EndCondition(1);
    }
    else if (threshold > 495.0f) {
        EndCondition(2);
    }
    //std::cout << game_objects_[7]->GetPosition()[1] << std::endl;
}

void Game::Controls(double delta_time)
{
    // Get player game object
    GameObject *player = game_objects_[0];
    // Get current position
    glm::vec3 curpos = player->GetPosition();
    // Set standard forward and right directions
    glm::vec3 dir = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 right = glm::vec3(1.0, 0.0, 0.0);
    // Adjust motion increment based on a given speed
    float speed = 3.5;
    float motion_increment = speed*delta_time;

    // Defining edges
    float leftEdge = -3.5f;
    float rightEdge = 3.5f;
    float topEdge = 3.5f;
    float bottomEdge = -3.5f;

    // Check for player input and make changes accordingly
    if (player->getFuel() > 0) {
        // Increasing forward movement when verified
        if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS && player->getVerified()) {
            player->SetPosition(curpos + motion_increment*dir);
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        }

        // Increasing normal forward movement
        else if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
            //player->SetPosition(curpos + motion_increment*dir);
            if (player->GetVelocity()[1] < 3.0f) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(0.0f, 0.025f, 0.0f));
            }

        }

        // Decreasing forward movement
        else if (glfwGetKey(window_, GLFW_KEY_S) != GLFW_PRESS) {
            if (player->GetVelocity()[1] > 0) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(0.0f, -0.025f, 0.0f));
            }
        }

        // Increasing backwards movement when verified
        if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS && player->getVerified()) {
            player->SetPosition(curpos - motion_increment*dir);
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

        }

        // Increasing backwards movement when verified
        else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
            //player->SetPosition(curpos - motion_increment*dir);
            if (player->GetVelocity()[1] > -3.0f) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(0.0f, -0.025f, 0.0f));
            }

        }

        // Decreasing backwards movement
        else if (glfwGetKey(window_, GLFW_KEY_W) != GLFW_PRESS) {
            if (player->GetVelocity()[1] < 0) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(0.0f, 0.025f, 0.0f));
            }
        }

        // Increasing right movement when verified
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS && player->getVerified()) {
            player->SetPosition(curpos + motion_increment*right);
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

        }

        // Increasing right movement
        else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
            //player->SetPosition(curpos + motion_increment*right);
            if (player->GetVelocity()[0] < 3.0f) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(0.025f, 0.0f, 0.0f));
            }

        }

        // Decreasing right movement
        else if (glfwGetKey(window_, GLFW_KEY_A) != GLFW_PRESS) {
            if (player->GetVelocity()[0] > 0) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(-0.025f, 0.0f, 0.0f));
            }
        }

        // Increasing left movement when verified
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS && player->getVerified()) {
            player->SetPosition(curpos - motion_increment*right);
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

        }

        // Increasing left movement
        else if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
            //player->SetPosition(curpos - motion_increment*right);
            if (player->GetVelocity()[0] > -3.0f) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(-0.025f, 0.0f, 0.0f));
            }

        }

        // Decreasing left movement
        else if (glfwGetKey(window_, GLFW_KEY_D) != GLFW_PRESS) {
            if (player->GetVelocity()[0] < 0) {
                player->SetVelocity(player->GetVelocity() + glm::vec3(0.025f, 0.0f, 0.0f));
            }
        }

    // No fuel left so player must come to a stop
    } else {

        // Decreasing forward movement
        if (player->GetVelocity()[1] > 0) {
            player->SetVelocity(player->GetVelocity() + glm::vec3(0.0f, -0.0025f, 0.0f));
        }

        // Decreasing backwards movement
        if (player->GetVelocity()[1] < 0) {
            player->SetVelocity(player->GetVelocity() + glm::vec3(0.0f, 0.0025f, 0.0f));
        }

        // Decreasing right movement
        if (player->GetVelocity()[0] > 0) {
            player->SetVelocity(player->GetVelocity() + glm::vec3(-0.0025f, 0.0f, 0.0f));
        }

        // Decreasing left movement
        if (player->GetVelocity()[0] < 0) {
            player->SetVelocity(player->GetVelocity() + glm::vec3(0.0025f, 0.0f, 0.0f));
        }
    }

    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // Checking to see if the cooldown permits shooting
        if (!player->space && current_time_ > player->bulCool && !player->getVerified()) {
            // Making a new bullet object to be fired
            BulletObject* bullet = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[6], 1.0f, dir, 7.0);
            //std::cout << current_time_ << std::endl;
            //std::cout << current_time_ + 5.0 << std::endl;
            bullet->SetRotate(player->GetRotate());
            bullet->SetAngle(player->GetAngle());
            bullet->SetPosition(player->GetPosition());
            bullet->SetScale(glm::vec3(0.25f, 0.25f, 1.0f));
            projectile_.push_back(bullet);

            // Setting the spacebar press to true
            player->bulCool = current_time_ + 0.5;
            player->space = true;

        } else if (!player->space && current_time_ > player->bulCool && player->getVerified()) {

            // Firing the first bullet
            BulletObject* firstBul = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[23], 1.0f, dir * 4.0f, 5.0f);
            firstBul->SetRotate(player->GetRotate());
            firstBul->SetAngle(player->GetAngle());
            firstBul->SetPosition(glm::vec3(player->GetPosition()[0] - 0.25f, player->GetPosition()[1], 0.0f));
            firstBul->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
            projectile_.push_back(firstBul);

            // Firing the second bullet
            BulletObject* secondBul = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[23], 1.0f, dir * 4.0f, 5.0f);
            secondBul->SetRotate(player->GetRotate());
            secondBul->SetAngle(player->GetAngle());
            secondBul->SetPosition(glm::vec3(player->GetPosition()[0] + 0.25f, player->GetPosition()[1], 0.0f));
            secondBul->SetScale(glm::vec3(0.25f, 0.25f, 0.25f));
            projectile_.push_back(secondBul);

            // Setting the spacebar press to true
            player->space = true;
            player->bulCool = current_time_;

        }
    } else {
        // Setting space bar
        player->space = false;
    }

    if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {

        if (current_time_ > player->gasCool && !player->getVerified() && player->getFuel() > 2.0f && player->shift == false) {
            // Creating fuel cannister
            BulletObject* gas = new BulletObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[21], 1.0f, dir, 3.0);
            //std::cout << current_time_ << std::endl;
            //std::cout << current_time_ + 5.0 << std::endl;
            gas->SetAngle(player->GetAngle());
            gas->SetPosition(player->GetPosition());
            gas->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
            gas->explode = true;
            gas->hit += 1.0f;
            projectile_.push_back(gas);

            // Subtracting fuel from the player
            player->setFuel(player->getFuel() - 1.0f);

            // Setting the cooldown time
            player->gasCool = current_time_ + 5.0f;
            player->shift = true;
        }

    } else {
        // Setting shift key
        player->shift = false;
    }

    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
}
       
} // namespace game
