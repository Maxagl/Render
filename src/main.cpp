#include <Gl/glew.h>
#include <GLFW/glfw3.h>


void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0);
}

int main(int argc, char ** argv)
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Opengl", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    while (!glfwWindowShouldClose(window))
    {
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    
    return 0;
}