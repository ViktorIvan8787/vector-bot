#ifndef RIGIDBODY_H
#define RIGIDBODY_H 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include "shader.hpp"

class RigidBody
{
public:
    glm::vec3 position;
    glm::vec3 scale = glm::vec3(1.0f);

    RigidBody(Mesh* mesh, Shader* shader, glm::vec3 position)
        : position(position), mesh(mesh), shader(shader) {}

    void Draw(const glm::mat4& view, const glm::mat4& projection)
    {   
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
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