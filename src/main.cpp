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
MeshRenderer* enemy;

btDiscreteDynamicsWorld* dynamicsWorld;

GLuint flatShaderProgram, litTextureShaderProgram, textProgram;
GLuint textureShaderProgram;
GLuint sphereTexture, groundTexture;

bool grounded;


void initGame();
void renderScene();
void addRigidBodies();
void myTickCallback(btDynamicsWorld* dynamicsWorld, btScalar timeStep);
void updateKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if(key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        if(grounded == true)
        {
            grounded = false;
            sphere->rigidBody->applyImpulse(btVector3(0.0f, 100.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));
            std::cout << "pressed up key" << std::endl;
        }
    }
}



void myTickCallback(btDynamicsWorld* dynamicsWorld, btScalar timeStep)
{
    btTransform t(enemy->rigidBody->getWorldTransform());
    t.setOrigin(t.getOrigin() + btVector3(-15, 0, 0) * timeStep);

    if(t.getOrigin().x() <= -18.0f)
    {
        t.setOrigin(btVector3(18, 1, 0));
    }
    enemy->rigidBody->setWorldTransform(t);
    enemy->rigidBody->getMotionState()->setWorldTransform(t);

    grounded = false;
    int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
    for(int i{0}; i < numManifolds; ++i)
    {
        btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        int numContacts = contactManifold->getNumContacts();
        if(numContacts > 0)
        {
            const btCollisionObject* objA = contactManifold->getBody0();
            const btCollisionObject* objB = contactManifold->getBody1();

            MeshRenderer* gModA = (MeshRenderer*)objA->getUserPointer();
            MeshRenderer* gModB = (MeshRenderer*)objB->getUserPointer();

            if((gModA->name == "hero" && gModB->name == "enemy") || (gModA->name == "enemy" && gModB->name == "hero"))
            {
                std::cout << "collision" << gModA->name << "with" << gModB->name;
                if(gModB->name == "enemy")
                {
                    btTransform b(gModB->rigidBody->getWorldTransform());
                    b.setOrigin(btVector3(18, 1, 0));
                    gModB->rigidBody->setWorldTransform(b);
                    gModB->rigidBody->getMotionState()->setWorldTransform(b);
                }
                else
                {
                    btTransform a(gModA->rigidBody->getWorldTransform());
                    a.setOrigin(btVector3(18, 1, 0));
                    gModA->rigidBody->setWorldTransform(a);
                    gModA->rigidBody->getMotionState()->setWorldTransform(a);                
                }
            }

            if((gModA->name == "hero" && gModB->name == "ground") || (gModA->name == "ground" && gModB->name == "hero"))
            {
                std::cout << "collision" << gModA->name << "with" << gModB->name;
                grounded = true;
            }


        }
    }


}

static void glfwError(int id, const char* description)
{
 std::cout << description << std::endl;
}
void addRigidBodies()
{
    btScalar mass = 13.0f;
	btVector3 sphereInertia(0, 0, 0);
    btCollisionShape* sphereShape = new btSphereShape(1.0f);
    // 后面btVector就是相对原点的初始位置，可以看btTransform构造体的说明
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
    // 告知这个body是被哪个指针所试用，后面碰撞的时候可以直接找到这个指针
    sphereRigidBody->setUserPointer(sphere);
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

    rb->setFriction(1.0f);
    rb->setRestitution(0.0f);
    rb->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
    dynamicsWorld->addRigidBody(rb);
    
    enemy = new MeshRenderer(MeshType::kCube, "enemy", camera, rb);
    enemy->setProgram(textureShaderProgram);
    enemy->setTexture(groundTexture);
    enemy->setScale(glm::vec3(1.0f, 1.0f, 1.0f));

    rb->setUserPointer(enemy);

    
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
    dynamicsWorld->setInternalTickCallback(myTickCallback);
    addRigidBodies();

}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 0.0f, 1.0);
    sphere->draw();
    ground->draw();
    enemy->draw();
}

int main(int argc, char ** argv)
{
    glfwSetErrorCallback(&glfwError);
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello Opengl", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, updateKeyboard);
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