#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

#include "rigidBody.hpp"

#include <cmath>
#include <array>

class Collision
{
public:
    void CollideWithGround(RigidBody& body, float groundY)
    {
        // Find lowest corner
        std::array<glm::vec3, 8> corners = body.GetCorners();
        glm::vec3 lowestCornerPos = corners[0];

        for (int i = 0; i < 8; i++)
        {
            if (corners[i].y <= lowestCornerPos.y)
            {
                lowestCornerPos = corners[i];
            }
        }

        if (lowestCornerPos.y <= groundY)
        {
            body.position.y += groundY - lowestCornerPos.y;
            body.velocity.y = 0;
        }
    }

private:

};

#endif