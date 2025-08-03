#include "Camera.h"

Camera::Camera(int width, int height, glm::vec2 position)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    glm::vec2 centerOffset = glm::vec2(width / 2.0f, height / 2.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-(Position - centerOffset), 0.0f));
    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window, double deltaTime)
{
    float move = speed * (float)deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position.y += move;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position.y -= move;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position.x -= move;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position.x += move;
}