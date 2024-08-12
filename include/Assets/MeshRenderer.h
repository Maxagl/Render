#include <vector>
#include "Assets/Camera.h"
#include "Assets/LightRenderer.h"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

class MeshRenderer
{
public:
    MeshRenderer(MeshType modelType, std::string _name, Camera* _camera, btRigidBody* _rigidBody);
    ~MeshRenderer();
    void draw();

    void setPosition(glm::vec3 _position);
    void setScale(glm::vec3 _scale);
    void setProgram(GLuint _program);
    void setTexture(GLuint _textureID);
    
private:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    glm::mat4 modelMatrix;
    std::string name = "";

    Camera* camera;
    glm::vec3 position;
    glm::vec3 scale;
    GLuint vao, vbo, ebo, texture, program;

public:
    btRigidBody* rigidBody;
};