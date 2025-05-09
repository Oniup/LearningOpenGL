#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common/Context.h"
#include "Common/Window.h"

namespace Cm
{
    struct Camera
    {
        enum ProjectionType
        {
            Perspective,
            Orthographic,
        };

        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Forward = glm::vec3(0.0f, 0.0f, -1);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        ProjectionType Projection = Perspective;
        float Fov = 55.0f;
        float Near = 0.1f;
        float Far = 100.0f;

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix(const Context& context);
    };

    struct FirstPersonCamera : Camera
    {
        static float MouseXOffset;
        static float MouseYOffset;
        static bool ImGuiEnabled;

        float Yaw = -90.0f;
        float Pitch = 0.0f;
        float MoveSpeed = 5.0f;
        float MouseSensitivity = 0.1f;
        bool CanRotate = true;

        FirstPersonCamera(const Context& context);

        void ProcessMovement(const Window& window, float deltaTime);
        void ProcessMouseMovement(bool constrainPitch = true);

        static void MouseCallback(GLFWwindow* window, double xPos, double yPos);
    };
}
