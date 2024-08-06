#include <Gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Assets/ShaderLoader.h"
#include "Assets/Camera.h"
#include "Assets/LightRenderer.h"
#include "Assets/MeshRenderer.h"
#include "Assets/TextureLoader.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <chrono>

Camera* camera;
LightRenderer* light;
MeshRenderer* sphere;
MeshRenderer* ground;

btDiscreteDynamicsWorld* dynamicsWorld;

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
    GLuint textureShaderProgram = shader.CreateProgram("../Shaders/TexturedModel.vs", "../Shaders/TexturedModel.fs");

    TextureLoader tLoader;
    GLuint sphereTexture = tLoader.getTextureID("../Images/globe.jpg");
    GLuint groundTexture = tLoader.getTextureID("../Images/ground.jpg");


    

    camera = new Camera(45.0f, 1920, 1080, 0.2f, 100.f, glm::vec3(0.0f, 4.0f, 20.0f));
    light = new LightRenderer(MeshType::kCube, camera);
    light->setProgram(flatShaderProgram);
    light->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* btCollisionConfiguration = new btDefaultCollisionConfiguration();;
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(btCollisionConfiguration);
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, btCollisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8, 0));
    btScalar mass = 10.0f;
    btVector3 sphereInertia(0.0f ,0.0f, 0.0f);

    btCollisionShape* sphereShape = new btSphereShape(1.0f);
    btDefaultMotionState* sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10.0f, 0)));
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    btRigidBody*  sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
    sphereRigidBody->setRestitution(1.0f);
    sphereRigidBody->setFriction(1.0f);
    sphereShape->calculateLocalInertia(mass, sphereInertia);

    btCollisionShape* groundShape = new btBoxShape(btVector3(4.0f, 0.5f, 4.0f));
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.0f, -2.0f, 0.0f)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0.0f, new btDefaultMotionState(), groundShape, btVector3(0.0f ,0.0f, 0.0f));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setFriction(1.0f);
    groundRigidBody->setRestitution(0.9);
    groundRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

    



    dynamicsWorld->addRigidBody(sphereRigidBody);
    dynamicsWorld->addRigidBody(groundRigidBody);

    sphere = new MeshRenderer(MeshType::kSphere, camera, sphereRigidBody);
    sphere->setProgram(textureShaderProgram);
    sphere->setTexture(sphereTexture);
    //sphere->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sphere->setScale(glm::vec3(1.0f));

    ground = new MeshRenderer(MeshType::kCube, camera, groundRigidBody);
    ground->setProgram(textureShaderProgram);
    ground->setTexture(groundTexture);
    ground->setScale(glm::vec3(4.0f, 0.5f, 4.0f));


}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0);
    sphere->draw();
    ground->draw();
}

int main(int argc, char ** argv)
{
    glfwSetErrorCallback(&glfwError);
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello Opengl", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    initGame();
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
        dynamicsWorld->stepSimulation(dt);
        renderScene();
        glfwSwapBuffers(window);
        glfwPollEvents();

        previousTime = currentTime;
    }
    glfwTerminate();
    
    delete camera;
    delete light;

    return 0;
}