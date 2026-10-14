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
    glm::quat orientation = glm::quat(1, 0, 0, 0);
    glm::vec3 angularVelocity = glm::vec3(0.0f);
    glm::vec3 invInertiaBody;
    glm::vec3 angularMomentum = glm::vec3(1.0f);
    

    float mass = 1.0f;
    float restitution = 0.8f;
    float friction = 0.8f;

    RigidBody(Mesh* mesh, Shader* shader, glm::vec3 position)
        : position(position), mesh(mesh), shader(shader) { ComputeInertia(); }

    void ComputeInertia()
    {
        float w = scale.x;
        float h = scale.y;
        float d = scale.z;
        float Ixx = mass/12 * (h*h + d*d);
        float Iyy = mass/12 * (w*w + d*d);
        float Izz = mass/12 * (w*w + h*h);
        invInertiaBody = glm::vec3(1.0f/Ixx, 1.0f/Iyy, 1.0f/Izz);
    }

    glm::mat3 GetInvInertiaWorld() const
    {
        glm::mat3 R = glm::mat3_cast(orientation);
        glm::mat3 invIBody(0.0f);
        invIBody[0][0] = invInertiaBody.x;
        invIBody[1][1] = invInertiaBody.y;
        invIBody[2][2] = invInertiaBody.z;
        return R * invIBody * glm::transpose(R);
    }

    std::array<glm::vec3, 8> GetCorners() const 
    {
        glm::mat3 R = glm::mat3_cast(orientation);
        std::array<glm::vec3, 8> corners;

        for (int i = 0; i < 8; i++)
        {
            float sx = (i & 1) ? 0.5f : -0.5f; // is bit 0 set
            float sy = (i & 2) ? 0.5f : -0.5f; // is bit 1 set
            float sz = (i & 4) ? 0.5f : -0.5f; // is bit 2 set - goes through all 8

            glm::vec3 local = glm::vec3(sx, sy, sz) * scale;
            corners[i] = position + R * local;
        }
        return corners;
    }

    void Update(float dt)
    {
        velocity += gravity * dt;
        position += velocity * dt;
        angularMomentum *= (1.0f - 0.1f * dt);

        // Rotation
        angularVelocity = GetInvInertiaWorld() * angularMomentum;

        glm::quat spin(0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z);
        orientation += 0.5f * dt * (spin * orientation);
        orientation = glm::normalize(orientation);
    }

    void Draw(const glm::mat4& view, const glm::mat4& projection)
    {   
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = model * glm::mat4_cast(orientation);
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