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

        if (lowestCornerPos.y >= groundY) return; // if not touching stop.

        body.position.y += groundY - lowestCornerPos.y; // push out

        corners = body.GetCorners(); // Get corners again to see how many impact

        std::array<float, 8> target{};
        std::array<float, 8> accumulated{};   // starts at 0
        glm::vec3 n(0.0f, 1.0f, 0.0f);

        // 1. targets, from the speeds before any impulse
        for (int i = 0; i < 8; i++)
        {
            if (corners[i].y > groundY + 0.001f) continue;
            glm::vec3 r = corners[i] - body.position;
            float vn0 = glm::dot(body.velocity + glm::cross(body.angularVelocity, r), n);
            target[i] = (vn0 < -0.5f) ? -body.restitution * vn0 : 0.0f;
        }

        // 2. repeat the solve
        for (int iter = 0; iter < 10; iter++)
        {
            for (int i = 0; i < 8; i++)
            {
                if (corners[i].y > groundY + 0.001f) continue;

                glm::vec3 r = corners[i] - body.position;
                float vn = glm::dot(body.velocity + glm::cross(body.angularVelocity, r), n);

                float denom = 1.0f / body.mass
                    + glm::dot(n, glm::cross(body.GetInvInertiaWorld() * glm::cross(r, n), r));

                float j = (target[i] - vn) / denom; // impulse needed to reach the target
                float newTotal = std::max(accumulated[i] + j, 0.0f);
                j = newTotal - accumulated[i]; // only the change
                accumulated[i] = newTotal;

                body.velocity        += (j / body.mass) * n;
                body.angularMomentum += glm::cross(r, j * n);
                body.angularVelocity  = body.GetInvInertiaWorld() * body.angularMomentum;
            }
        }
    }

private:

};

#endif