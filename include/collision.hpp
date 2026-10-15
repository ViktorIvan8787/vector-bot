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
        glm::vec3 normalGround(0.0f, 1.0f, 0.0f);

        // 1. targets, from the speeds before any impulse
        for (int i = 0; i < 8; i++)
        {
            if (corners[i].y > groundY + 0.001f) continue;
            glm::vec3 leverArm = corners[i] - body.position;
            float cornerApproachSpeed = glm::dot(body.velocity + glm::cross(body.angularVelocity, leverArm), normalGround);
            target[i] = (cornerApproachSpeed < -0.5f) ? -body.restitution * cornerApproachSpeed : 0.0f;
        }

        // 2. repeat the solve
        for (int iter = 0; iter < 10; iter++)
        {
            for (int i = 0; i < 8; i++)
            {
                if (corners[i].y > groundY + 0.001f) continue;

                glm::vec3 leverArm = corners[i] - body.position;
                float approachSpeed = glm::dot(body.velocity + glm::cross(body.angularVelocity, leverArm), normalGround);

                float denominator = 1.0f / body.mass
                    + glm::dot(normalGround, glm::cross(body.GetInvInertiaWorld() * glm::cross(leverArm, normalGround), leverArm));

                float impulse = (target[i] - approachSpeed) / denominator; // impulse needed to reach the target
                float newTotal = std::max(accumulated[i] + impulse, 0.0f);
                impulse = newTotal - accumulated[i]; // only the change
                accumulated[i] = newTotal;

                body.velocity        += (impulse / body.mass) * normalGround;
                body.angularMomentum += glm::cross(leverArm, impulse * normalGround);
                body.angularVelocity  = body.GetInvInertiaWorld() * body.angularMomentum;

                // implementing friction
                glm::vec3 cornerVelocityNew = body.velocity + glm::cross(body.angularVelocity, leverArm);
                glm::vec3 sidewaysVelocity = cornerVelocityNew - glm::dot(cornerVelocityNew, normalGround) * normalGround; // friction only from parallel to ground forces
                if (glm::length(sidewaysVelocity) >= 0.01f)
                {
                    glm::vec3 slideDirection = glm::normalize(sidewaysVelocity);
                    // how much does box move per unit of push?
                    float denominatorTangent = 1 / body.mass + glm::dot(slideDirection, glm::cross(body.GetInvInertiaWorld() * glm::cross(leverArm, slideDirection), leverArm));
                    float impulseTangent = glm::length(sidewaysVelocity) / denominatorTangent;
                    impulseTangent = std::min(impulseTangent, body.friction * accumulated[i]);

                    glm::vec3 frictionalForce = -impulseTangent * slideDirection;
                    body.velocity += frictionalForce / body.mass;
                    body.angularMomentum += glm::cross(leverArm, frictionalForce); // torque from friction
                    body.angularVelocity = body.GetInvInertiaWorld() * body.angularMomentum;
                }

            }
        }
    }

private:

};

#endif