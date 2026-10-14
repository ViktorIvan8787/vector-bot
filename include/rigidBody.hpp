#ifndef RIGIDBODY_H
#define RIGIDBODY_H 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "mesh.hpp"
#include "shader.hpp"

#include <array>

class RigidBody
{
public:
    glm::vec3 position;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 angularVelocity = glm::vec3(0.0f);
    glm::vec3 inverseInertiaBody; // for spin based off angular momentum
    glm::vec3 angularMomentum = glm::vec3(0.0f, 0.0f, 0.0f);

    float mass = 1.0f;
    float restitution = 0.8f;

    RigidBody(Mesh* mesh, Shader* shader, glm::vec3 position)
        : position(position), mesh(mesh), shader(shader) { ComputeInertia(); }

    void ComputeInertia()
    {
        float w = scale.x; // width
        float h = scale.y; // height
        float d = scale.z; // depth
        float Inertiaxx = mass/12 * (h*h + d*d);
        float Inertiayy = mass/12 * (w*w + d*d);
        float Inertiazz = mass/12 * (w*w + h*h);
        inverseInertiaBody = glm::vec3(1/Inertiaxx, 1/Inertiayy, 1/Inertiazz);
    }

    glm::mat3 GetInvInertiaWorld() const
    {
        glm::mat3 rotationMatrix = glm::mat3_cast(orientation);
        glm::mat3 inverseInertiaBodyDiag = glm::mat3(0.0f);
        inverseInertiaBodyDiag[0][0] = inverseInertiaBody.x;
        inverseInertiaBodyDiag[1][1] = inverseInertiaBody.y;
        inverseInertiaBodyDiag[2][2] = inverseInertiaBody.z;
        return rotationMatrix * inverseInertiaBodyDiag * glm::transpose(rotationMatrix);
    }

    std::array<glm::vec3, 8> GetCorners() const
    {
        glm::mat3 rotationMatrix = glm::mat3_cast(orientation);
        std::array<glm::vec3, 8> corners;

        for (int i = 0; i < 8; i++)
        {
            // Iterate through sign combinations 0.5 & -0.5 to find positions
            // of corners (they are 0.5 away) * scale. Uses i to find every combination.
            float signx = (i & 1) ? 0.5f : -0.5f;
            float signy = (i & 2) ? 0.5f : -0.5f;
            float signz = (i & 4) ? 0.5f : -0.5f;
            glm::vec3 signs = glm::vec3(signx, signy, signz);
            corners[i] = position + rotationMatrix * (glm::vec3(signs) * scale);
        }
        return corners;
    }

    void Update(float dt)
    {   
        // update velocity and position
        velocity += gravity * dt;
        position += velocity * dt;
        angularMomentum *= (1.0f - 0.5f * dt);                          // Energy loss present here
        angularVelocity = GetInvInertiaWorld() * angularMomentum;

        // update orientation
        glm::quat spin(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
        orientation += 0.5f * dt * (spin * orientation);
        orientation = glm::normalize(orientation);
    }

    void Draw(const glm::mat4& view, const glm::mat4& projection)
    {   
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = model * glm::mat4_cast(orientation); // turn to 4x4 rotation matrix
        model = glm::scale(model, scale);

        shader->use();
        shader->setMat4("model", model);
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        mesh->Draw();
    }
private:
    Mesh* mesh;
    Shader* shader;
};

#endif