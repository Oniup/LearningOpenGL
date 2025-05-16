#include "Common/Camera.h"

#include <imgui/backends/imgui_impl_glfw.h>

namespace Cm
{
    float FirstPersonCamera::MouseXOffset = 0.0f;
    float FirstPersonCamera::MouseYOffset = 0.0f;
    bool FirstPersonCamera::ImGuiEnabled = false;

    glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position - Forward, Up);
    }

    glm::mat4 Camera::GetProjectionMatrix(const Context& context)
    {
        int width, height;
        context.GetWindow().GetSize(&width, &height);

        if (Projection == Perspective)
            return glm::perspective(glm::radians(Fov), (float)width / (float)height, Near, Far);

        return glm::mat4(0.0f);
    }

    FirstPersonCamera::FirstPersonCamera(const Context& context)
    {
        glfwSetCursorPosCallback(context.GetWindow().GetInternalWindow(), Cm::FirstPersonCamera::MouseCallback);
        glfwSetInputMode(context.GetWindow().GetInternalWindow(), GLFW_CURSOR, CanRotate ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        ImGuiEnabled = context.IsImGuiEnabled();
    }

    void FirstPersonCamera::ProcessMovement(const Context& context)
    {
        glm::vec3 moveDirection(0.0f);
        float moveSpeed = MoveSpeed;

        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_W) == GLFW_PRESS)
            moveDirection += -Forward;
        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_S) == GLFW_PRESS)
            moveDirection += Forward;
        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_A) == GLFW_PRESS)
            moveDirection += glm::cross(Forward, Up);
        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_D) == GLFW_PRESS)
            moveDirection += -glm::cross(Forward, Up);
        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_E) == GLFW_PRESS)
            moveDirection += Up;
        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_Q) == GLFW_PRESS)
            moveDirection += -Up;
        if (glfwGetKey(context.GetWindow().GetInternalWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            moveSpeed *= 2;

        if (glm::dot(moveDirection, moveDirection) != 0)
            Position += glm::normalize(moveDirection) * moveSpeed * context.GetDeltaTime();

        static bool last = true;
        bool switchCanRotate = glfwGetMouseButton(context.GetWindow().GetInternalWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        if (last != switchCanRotate)
        {
            CanRotate = !CanRotate;
            glfwSetInputMode(context.GetWindow().GetInternalWindow(), GLFW_CURSOR, CanRotate ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        last = switchCanRotate;
    }

    void FirstPersonCamera::ProcessMouseMovement(bool constrainPitch)
    {
        if (CanRotate)
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
        }
        MouseXOffset = 0.0f;
        MouseYOffset = 0.0f;
   }

    void FirstPersonCamera::MouseCallback(GLFWwindow* window, double xPos, double yPos)
    {
        if (ImGuiEnabled)
            ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);

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
