#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Assets/ShaderLoader.h"
#include "Assets/Camera.h"
#include "Assets/LightRenderer.h"

Camera* camera;
LightRenderer* light;

void initGame();
void renderScene();

static void glfwError(int id, const char* description)
{
 std::cout << description << std::endl;
}

void initGame()
{
    glEnable(GL_DEPTH_TEST);
    ShaderLoader shader;
    GLuint flatShaderProgram = shader.CreateProgram("../Shaders/FlatModel.vs", "../Shaders/FlatModel.fs");

    camera = new Camera(45.0f, 1920, 1080, 0.2f, 100.f, glm::vec3(4.0f, 4.0f, 6.0f));
    light = new LightRenderer(MeshType::kCube, camera);
    light->setProgram(flatShaderProgram);
    light->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0);
    light->draw();
}

int main(int argc, char ** argv)
{
    glfwSetErrorCallback(&glfwError);
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello Opengl", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    initGame();
    while (!glfwWindowShouldClose(window))
    {
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    
    delete camera;
    delete light;

    return 0;
}