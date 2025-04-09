#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common/Window.h"

namespace Cm
{
    struct Camera
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Forward = glm::vec3(0.0f, 0.0f, -1);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 GetViewModel();
    };

    struct FirstPersonCamera : Camera
    {
        float Yaw = -90.0f;
        float Pitch = 0.0f;
        float MoveSpeed = 5.0f;
        float MouseSensitivity = 0.1f;

        void ProcessMovement(Window& window, float deltaTime);
        void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    };
}
