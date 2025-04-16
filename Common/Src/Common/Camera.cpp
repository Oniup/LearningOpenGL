#include "Common/Camera.h"

namespace Cm
{
    float FirstPersonCamera::MouseXOffset = 0.0f;
    float FirstPersonCamera::MouseYOffset = 0.0f;

    glm::mat4 Camera::GetViewModel()
    {
        return glm::lookAt(Position, Position - Forward, Up);
    }

    void FirstPersonCamera::ProcessMovement(Window& window, float deltaTime)
    {
        glm::vec3 moveDirection(0.0f);
        float moveSpeed = MoveSpeed;

        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_W) == GLFW_PRESS)
            moveDirection += -Forward;
        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_S) == GLFW_PRESS)
            moveDirection += Forward;
        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_A) == GLFW_PRESS)
            moveDirection += glm::cross(Forward, Up);
        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_D) == GLFW_PRESS)
            moveDirection += -glm::cross(Forward, Up);
        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_E) == GLFW_PRESS)
            moveDirection += Up;
        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_Q) == GLFW_PRESS)
            moveDirection += -Up;
        if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            moveSpeed *= 2;

        if (glm::dot(moveDirection, moveDirection) != 0)
            Position += glm::normalize(moveDirection) * moveSpeed * deltaTime;
    }

    void FirstPersonCamera::ProcessMouseMovement(bool constrainPitch)
    {
        MouseXOffset *= MouseSensitivity;
        MouseYOffset *= MouseSensitivity;

        Yaw   += MouseXOffset;
        Pitch -= MouseYOffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        glm::vec3 direction;
        float yaw = glm::radians(Yaw);
        float pitch = glm::radians(Pitch);
        direction.x = cos(yaw) * cos(pitch);
        direction.y = sin(pitch);
        direction.z = sin(yaw) * cos(pitch);
        Forward = glm::normalize(direction);

        MouseXOffset = 0.0f;
        MouseYOffset = 0.0f;
    }

    void FirstPersonCamera::MouseCallback(GLFWwindow* window, double xPos, double yPos)
    {
        static bool firstMouse = true;
        static float lastX = 0.0f;
        static float lastY = 0.0f;
        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
            return;
        }

        MouseXOffset = xPos - lastX;
        MouseYOffset = lastY - yPos;
        lastX = xPos;
        lastY = yPos;
    }
}
