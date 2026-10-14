#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

#include "rigidBody.hpp"

#include <array>
#include <algorithm>

class Collision
{
public:
    void CollideWithGround(RigidBody& body, float groundY)
    {
        // Works by finding impulse with the ground based off of mass,
        // inertia, distance between the corners and the current velocity.

        auto corners = body.GetCorners();
        float lowest = corners[0].y;
        for (int i = 1; i < 8; i++) // find lowest corner
            lowest = std::min(lowest, corners[i].y);
        if (lowest >= groundY) return;

        body.position.y += groundY - lowest;
        corners = body.GetCorners();

        // impulse
        for (int i = 0; i < 8; i++)
        {
            if (corners[i].y > groundY + 0.001f) continue;

            // contact point, lever arm, velocity of contact point
            glm::vec3 contact = corners[i];
            contact.y = groundY;
            glm::vec3 leverArm = contact - body.position;
            glm::vec3 n(0.0f, 1.0f, 0.0f);

            glm::vec3 vp = body.velocity + glm::cross(body.angularVelocity, leverArm);
            float vn = glm::dot(vp, n);  

            // impulse
            glm::mat3 invIertia = body.GetInvInertiaWorld();
            float denominator = 1.0f / body.mass + glm::dot(n, glm::cross(invIertia * glm::cross(leverArm, n), leverArm));
            float impulse = -(1.0f + body.restitution) * vn / denominator;

            // friction
            glm::vec3 vp2 = body.velocity + glm::cross(body.angularVelocity, leverArm);
            glm::vec3 vt = vp2 - glm::dot(vp2, n) * n; // sliding tanget part of the corner
            float vtLen = glm::length(vt);
            if (vtLen > 1e-5f)
            {
                glm::vec3 tangent = vt / vtLen;
                float denominatorT = 1.0f / body.mass + glm::dot(tangent, glm::cross(body.GetInvInertiaWorld() * glm::cross(leverArm, tangent), leverArm));
                float impulseT = vtLen / denominatorT; // impulse to stop slide
                impulseT = std::min(impulseT, body.friction * impulse);
                glm::vec3 F = -impulseT * tangent;
                body.velocity += F / body.mass;
                body.angularMomentum += glm::cross(leverArm, F);
                body.angularVelocity = body.GetInvInertiaWorld() * body.angularMomentum;
            }

            body.velocity += (impulse / body.mass) * n; 
            body.angularMomentum += glm::cross(leverArm, impulse * n);

            body.angularVelocity = body.GetInvInertiaWorld() * body.angularMomentum;
        }

    }

private:

};

#endif 