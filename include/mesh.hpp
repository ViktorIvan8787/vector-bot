#ifndef MESH_H 
#define MESH_H

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

class Mesh
{
public:
    Mesh(const std::vector<float>& vertices, const std::vector<int>& attributeSizes)
    {
        int stride = 0;
        for (int size : attributeSizes) stride += size; // Find stride by adding num attribs

        vertexCount = static_cast<int>(vertices.size()) / stride;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        
        int offset = 0;
        for (size_t i = 0; i < attributeSizes.size(); ++i)
        {
            glVertexAttribPointer(
                static_cast<unsigned int>(i),
                attributeSizes[i],
                GL_FLOAT,
                GL_FALSE,
                stride * sizeof(float),
                (void*)(offset * sizeof(float))
            );
            glEnableVertexAttribArray(static_cast<unsigned int>(i));
            offset += attributeSizes[i];
        }
    }
    void Draw() const
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

private:
    unsigned int VAO, VBO;
    int vertexCount;
};


#endif