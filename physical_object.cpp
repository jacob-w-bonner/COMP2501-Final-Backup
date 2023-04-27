#include "physical_object.h"
#include "game_object.h"
#include "math_object.h"

namespace game {

/*
    physicalobject inherits from GameObject
    It implements Radius, which is used for collision detection. 
*/

PhysicalObject::PhysicalObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float radius) 
 :  GameObject(position, geom, shader,texture)
{
    radius_ = radius;
    deceased = 0.0f;
}
float PhysicalObject::getRadius()
{
    return radius_;
}
glm::vec3 PhysicalObject::getPosition()
{
    return position_;
}

glm::vec3 PhysicalObject::setRandomSpawnLocation(glm::vec3 playerLocation)
{
    glm::vec3 spawnLocation;
    do {
        spawnLocation = MathObject::getRandomPosition2D(15, 15);
    } while (glm::distance(spawnLocation, playerLocation) <= 1.5f);
    return spawnLocation;
}

} // namespace game