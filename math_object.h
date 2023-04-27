#pragma once
#ifndef MATHOBJECT_H_
#define MATHOBJECT_H_

#include <cmath>
#include <glm/glm.hpp>


namespace game {

    // Inherits from GameObject
    class MathObject{

    public:
        MathObject();
        static float Pythagorean(float, float, float, float);
        static float Pythagorean(glm::vec3, glm::vec3);
        static glm::vec3 findVelocity(glm::vec3, glm::vec3, float);
        static glm::vec3 findVelocityFromAngleSpeed(float, float);
        static glm::vec3 getRandomPosition2D(int, int);
        static double getAngleToPoint(glm::vec3, glm::vec3);
        static double pi();
        static double decelerationSpeed();
    protected:

    }; // class PhysicalObject

} // namespace game

#endif // PHYSICALOBJECT_H_
