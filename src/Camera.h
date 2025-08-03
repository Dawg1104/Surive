#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Camera
{
public:
    glm::vec2 Position;
    int width;
    int height;
    float speed = 3200.0f; // pixels per second

    Camera(int width, int height, glm::vec2 position);

    void Matrix(Shader& shader, const char* uniform);
    void Inputs(GLFWwindow* window, double deltaTime);
};

#endif