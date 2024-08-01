#include "Assets/MeshRenderer.h"


MeshRenderer::MeshRenderer(MeshType modelType, Camera* _camera)
{
    camera = _camera;
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    switch (modelType)
    {
    case kTriangle:
        Mesh::setTriData(vertices, indices);
        break;
    case kQuad:
        Mesh::setQuadData(vertices, indices);
        break;
    case kCube:
        Mesh::setCubeData(vertices, indices);
        break;
    case kSphere:
        Mesh::setSphereData(vertices, indices);
        break;
    }
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    
    glEnableVertexArrayAttrib(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::texCoords)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
MeshRenderer::~MeshRenderer()
void MeshRenderer::draw()

void MeshRenderer::setPosition(glm::vec3 _position)
void MeshRenderer::setScale(glm::vec3 _scale)
void MeshRenderer::setProgram(GLuint _program)
void MeshRenderer::setTexture(GLuint _textureID)