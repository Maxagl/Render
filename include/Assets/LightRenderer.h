#pragma once

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Assets/Mesh.h"
#include "Assets/ShaderLoader.h"
#include "Assets/Camera.h"

class LightRenderer
{
private:
    Camera* camera;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    glm::vec3 position;
    glm::vec3 color;
    GLuint vbo, ebo, vao, program;


public:
    LightRenderer(MeshType MeshType, Camera* camera);
    ~LightRenderer();

    void draw();

    void setPosition(glm::vec3 _position);
    void setColor(glm::vec3 _color);
    void setProgram(GLuint program);

    glm::vec3 getPosition();
    glm::vec3 getColor();

};