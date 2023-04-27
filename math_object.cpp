#include "math_object.h"
#include <math.h>

namespace game {

    /*
        collectible_object inherits from physical_object
        It implements Radius, which is used for collision detection.
    */

    MathObject::MathObject()
    {

    }

    float MathObject::Pythagorean(float x1, float y1, float x2, float y2)
    {
        float xlength = abs(x2 - x1);
        float ylength = abs(y2 - y1);
        float sumOfSquares = (xlength * xlength) + (ylength * ylength);
        return (sqrt(sumOfSquares));
    }
    float MathObject::Pythagorean(glm::vec3 point1, glm::vec3 point2)
    {
        float xlength = abs(point2.x - point1.x);
        float ylength = abs(point2.y - point1.y);
        float sumOfSquares = (xlength * xlength) + (ylength * ylength);
        return (sqrt(sumOfSquares));
    }

    glm::vec3 MathObject::findVelocity(glm::vec3 currentPos, glm::vec3 targetPos, float speedPerSecond)
    {
        glm::vec3 targetVector = targetPos - currentPos;
        glm::vec3 normalizedTargetVector = targetVector / glm::length(targetVector);
        glm::vec3 velocityVector = normalizedTargetVector * speedPerSecond;
        return velocityVector;
    }

    glm::vec3 MathObject::findVelocityFromAngleSpeed(float angle, float speed)
    {
        return glm::vec3(speed * cos(angle), speed * sin(angle), 0);
    }

    glm::vec3 MathObject::getRandomPosition2D(int xBound, int yBound)
    {
        int xbase = ((rand() % (200 * xBound)) - (100 * xBound));
        int ybase = ((rand() % (200 * yBound)) - (100 * yBound));
        float xPosition = xbase / static_cast<float>(100);
        float yPosition = ybase / static_cast<float>(100);
        glm::vec3 position = glm::vec3(xPosition, yPosition, 0);
        return position;
    }

    double MathObject::getAngleToPoint(glm::vec3 currentPoint, glm::vec3 targetPoint)
    {
        double yDiff = currentPoint.y - targetPoint.y;
        double xDiff = currentPoint.x - targetPoint.x;
        double finalAngle = atan2(yDiff, xDiff);
        return finalAngle;
    }

    double MathObject::pi()
    {
        return 3.14159;
    }

    double MathObject::decelerationSpeed()
    {
        return 0.6;
    }

} // namespace game
