#include "Assets/MeshRenderer.h"

MeshRenderer::MeshRenderer(MeshType modelType, std::string _name, Camera* _camera, btRigidBody* _rigidBody)
{
    rigidBody = _rigidBody;
    camera = _camera;
    name = _name;
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // 初始位置都是0，后面控制都由bullet来控制
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
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, Vertex::texCoords)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
MeshRenderer::~MeshRenderer()
{

}
void MeshRenderer::draw()
{
    btTransform t;
    rigidBody->getMotionState()->getWorldTransform(t);
    // 直接找到角度和translattion就结束了，这也太简单了吧
    btQuaternion rotation = t.getRotation();
    // 这里找到的，就是前面bullet的state里面设置的原点。这个原点就是平移的点
    btVector3 translate = t.getOrigin();

    glm::mat4 RotationMatrix = glm::rotate(glm::mat4(1.0f), rotation.getAngle(), 
    glm::vec3(rotation.getAxis().getX(), rotation.getAxis().getY(), rotation.getAxis().getZ()));
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translate.getX(), translate.getY(), translate.getZ()));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);


    modelMatrix = translationMatrix * RotationMatrix * scaleMatrix;
    glm::mat4 vp = camera->getProjectionMatrix() * camera->getViewMatrix();

    glUseProgram(this->program);
    GLint vpLoc = glGetUniformLocation(program, "vp");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(vp));

    GLint modelLoc = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindTexture(GL_TEXTURE_2D, texture);

    // vao 相当于一个固定块，或者指针。激活相当于把VBO，EBO那些操作干一遍
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

void MeshRenderer::setPosition(glm::vec3 _position)
{
    position = _position;
}
void MeshRenderer::setScale(glm::vec3 _scale)
{
    scale = _scale;
}
void MeshRenderer::setProgram(GLuint _program)
{
    program = _program;
}
void MeshRenderer::setTexture(GLuint _textureID)
{
    texture = _textureID;
}