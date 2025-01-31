#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 cameraPos;
public:
    // field ofview (FOV), distance to nearPlane, distance to farPlane
    Camera(GLfloat FOV, GLfloat width, GLfloat height, GLfloat nearPlane, GLfloat farPlanem, glm::vec3 camPos);
    ~Camera();

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    glm::vec3 getCameraPosition();

};