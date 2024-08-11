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

GLuint flatShaderProgram, litTextureShaderProgram, textProgram;
GLuint textureShaderProgram;
GLuint sphereTexture, groundTexture;


void initGame();
void renderScene();
void addRigidBodies();

static void glfwError(int id, const char* description)
{
 std::cout << description << std::endl;
}
void addRigidBodies()
{
    btScalar mass = 13.0f;
	btVector3 sphereInertia(0, 0, 0);
    btCollisionShape* sphereShape = new btSphereShape(1.0f);
    btDefaultMotionState* sphereMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0.5f, 0)));
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    btRigidBody*  sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
    sphereRigidBody->setRestitution(1.0f);
    sphereRigidBody->setFriction(0.0f);
    //sphereShape->calculateLocalInertia(mass, sphereInertia);
    sphereRigidBody->setActivationState(DISABLE_DEACTIVATION);

    sphere = new MeshRenderer(MeshType::kSphere, "hero", camera, sphereRigidBody);
    sphere->setProgram(textureShaderProgram);
    sphere->setTexture(sphereTexture);
    //sphere->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    sphere->setScale(glm::vec3(1.0f));
    dynamicsWorld->addRigidBody(sphereRigidBody);

    // github官网文档有介绍MotionState是什么
    btCollisionShape* groundShape = new btBoxShape(btVector3(4.0f, 0.5f, 4.0f));
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.0f, -2.0f, 0.0f)));
    btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0.0f, new btDefaultMotionState(), groundShape, btVector3(0.0f ,0.0f, 0.0f));
    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setFriction(1.0f);
    groundRigidBody->setRestitution(0.0);
    groundRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

    ground = new MeshRenderer(MeshType::kCube, "ground", camera, groundRigidBody);
    ground->setProgram(textureShaderProgram);
    ground->setTexture(groundTexture);
    ground->setScale(glm::vec3(4.0f, 0.5f, 4.0f));
    groundRigidBody->setUserPointer(ground);

    dynamicsWorld->addRigidBody(groundRigidBody);

    // Enemy Rigid body
    btCollisionShape* shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(18.0f, 1.0f, 0.0f)));
    btRigidBody::btRigidBodyConstructionInfo rbCI(0.0f, motionState, shape, btVector3(0.0f, 0.0f, 0.0f));
    btRigidBody* rb = new btRigidBody(rbCI);

}
void initGame()
{
    glEnable(GL_DEPTH_TEST);
    ShaderLoader shader;
    flatShaderProgram = shader.CreateProgram("../Shaders/FlatModel.vs", "../Shaders/FlatModel.fs");
    textureShaderProgram = shader.CreateProgram("../Shaders/TexturedModel.vs", "../Shaders/TexturedModel.fs");

    TextureLoader tLoader;
    sphereTexture = tLoader.getTextureID("../Images/globe.jpg");
    groundTexture = tLoader.getTextureID("../Images/ground.jpg");


    

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
    addRigidBodies();

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