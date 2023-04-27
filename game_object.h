#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include "shader.h"
#include "geometry.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture);

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            //used for collision/proximity detection	
            float getDistanceToPoint(glm::vec3 targetPos);

            // Getters
            inline glm::vec3& GetPosition(void) { return position_; }
            inline glm::vec3& GetScale(void) { return scale_; }
            inline glm::vec3& GetVelocity(void) { return velocity_; }
            inline glm::vec3& GetRotation(void) { return roPoint; }

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(glm::vec3 scale) { scale_ = scale; }

            inline void SetVelocity(const glm::vec3& velocity) { velocity_ = velocity; }

            // Object hostility value
            bool hostile_ = false;

            // The health of the object
            int health;

            // Object shield health
            int shield;

            // Stores the state of whether or not object is tracking player
            bool state;

            // The player location
            glm::vec3 player;

            // Rotation point
            glm::vec3 roPoint;

            // Rotation angle
            float rotAngle;

            // Getter and setter for fuel
            inline float getFuel(void) { return fuel; }
            inline void setFuel(float f) { fuel = f; }

            // Getter for the type of object
            inline int getType(void) { return type; }

            // Getter and setter for if the objects are verified
            inline bool getVerified(void) { return verified; }
            inline void setVerified(bool b) { verified = b; }

            // Tile number
            int tileNum;

            // Getter for the rotation matrix
            inline glm::mat4 GetRotate() { return rotate_; };

            // Setter for the rotation matrix
            void SetRotate(glm::mat4 m) { rotate_ = m; };

            // Getter and setter for mass
            inline int GetMass(void) { return mass; }
            inline void SetMass(int m) { mass = m; }

            // Getter and setter for angles
            inline float GetAngle(void) { return angle; }
            inline void SetAngle(float a) { angle = a; }

            // Collision checking
            bool collision;

            // Spacebar press
            bool space;

            // Shift press
            bool shift;

            // Shooting cooldown
            double bulCool;

            // Gas can cooldown
            double gasCool;

            // Signifier that an object should be explosive upon destruction
            bool explode;

            // Setting a red value
            float red;

            // Setting a powerup timer
            double inv;

            // Setting a deletion time
            double despawn;

            // Hit tracker for the shader
            float hit;

        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            glm::vec3 scale_;
            glm::vec3 velocity_;
            // TODO: Add more transformation variables

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

            // Keeping track of fuel
            float fuel;

            // Tracking what kind of object is created
            int type;

            // Tracking if an item is powered up
            bool verified;

            // The rotation matrix
            glm::mat4 rotate_;

            // Holding the mass
            int mass;

            // Angle of rotation
            float angle;

            // Time the object should die
            double deceased;


    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
