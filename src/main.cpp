#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "rigidBody.hpp"
#include "geometry.hpp"

#include <iostream>
#include <cmath>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // resize window when updated
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
float randomFloat(float min, float max);
GLFWwindow* initWindow();
void drawGround(Shader& gridFloow, Mesh& groundMesh, const glm::mat4& view, const glm::mat4& projection);
void drawLightCube(Shader& lightShader, Mesh& cubeMesh, const glm::mat4& view, const glm::mat4& projection);
void drawCoralCube(Shader& smallCoralShader, RigidBody& coral, const glm::mat4& view, const glm::mat4& projection);

Camera camera(glm::vec3(5.0f, 5.0f, 5.0f));

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

bool rightMouseHeld = false;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastMouseX = SCR_WIDTH/2.0f; // middle of window
float lastMouseY = SCR_HEIGHT/2.0f;

// light
glm::vec3 lightPos(50.0f, 80.0f, 50.0f);
glm::vec3 smallCoralPos(2.0f, 1.6f, 2.6f);


int main()
{
    GLFWwindow* window = initWindow();
    if (window == NULL)
        return -1;
 
    // enables
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
    // Build shader program
    Shader gridFloor("shaders/gridFloor.vs", "shaders/gridFloor.fs");
    Shader lightShader("shaders/lightShader.vs", "shaders/lightShader.fs");
    Shader smallCoralShader("shaders/smallCoralShader.vs", "shaders/smallCoralShader.fs");
    
    camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
 
    // meshes
    Mesh cubeMesh(cubeVertices, {3, 3, 2});
    Mesh groundMesh(groundVertices, {3, 3, 2});
    // bodies
    RigidBody coral(&cubeMesh, &smallCoralShader, smallCoralPos);
 
    gridFloor.use();
    gridFloor.setFloat("gridSpacing", 1.0f);
    gridFloor.setFloat("lineWidth", 0.02f);
    gridFloor.setVec3("gridColor", glm::vec3(0.5f));
 
 
    // Window while loop
    while(!glfwWindowShouldClose(window))
    {
        // frames
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
 
        // input
        processInput(window);
 
        // rendering
        glClearColor(0.059f, 0.063f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear depth and color each iteration
 
        // projection and view
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();
 
        // ground
        drawGround(gridFloor, groundMesh, view, projection);
 
        // light Cube
        drawLightCube(lightShader, cubeMesh, view, projection);
 
        // coral cube
        drawCoralCube(smallCoralShader, coral, view, projection);
        
        // check&call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwTerminate();
    return 0;
}
 
GLFWwindow* initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // If using mac
#endif
 
    // width, height, name, monitor num, share
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Physics Robot Simulation", NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // resizing
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
 
    // Initialize GLAD as it manages function pointers for OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }
 
    return window;
}
 
void drawGround(Shader& gridFloor, Mesh& groundMesh, const glm::mat4& view, const glm::mat4& projection)
{
    gridFloor.use();
    gridFloor.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    gridFloor.setVec3("lightPos", lightPos);
    gridFloor.setVec3("viewPos", camera.Position);
    gridFloor.setMat4("projection", projection);
    gridFloor.setMat4("view", view);
    glm::mat4 groundModel = glm::mat4(1.0f);
    gridFloor.setMat4("model", groundModel);
    groundMesh.Draw();
}
 
void drawLightCube(Shader& lightShader, Mesh& cubeMesh, const glm::mat4& view, const glm::mat4& projection)
{
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(3.0f)); 
    lightShader.setMat4("model", lightModel);
    cubeMesh.Draw();
}
 
void drawCoralCube(Shader& smallCoralShader, RigidBody& coral, const glm::mat4& view, const glm::mat4& projection)
{
    smallCoralShader.use();
    smallCoralShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    smallCoralShader.setVec3("lightColor", glm::vec3(1.0f));
    smallCoralShader.setVec3("lightPos", lightPos);
    smallCoralShader.setVec3("viewPos", camera.Position);
    coral.scale = glm::vec3(0.5f + 0.5f * sin(glfwGetTime()));
    coral.Draw(view, projection);
}
 
void processInput(GLFWwindow *window)
{   
    // Exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
 
    // Camera
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        rightMouseHeld = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else 
    {
        rightMouseHeld = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyBoard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyBoard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyBoard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyBoard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyBoard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyBoard(UP, deltaTime);
}
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
 
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{    
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
 
    if (!rightMouseHeld)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        return;
    }
 
    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // y axis is flipped
 
    lastMouseX = xpos;
    lastMouseY = ypos;
 
    camera.ProcessMouseMovement(xoffset, yoffset);
}
 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) 
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
 
float randomFloat(float min, float max)
{
    static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> ditribution(min, max);
    return ditribution(generator);
}