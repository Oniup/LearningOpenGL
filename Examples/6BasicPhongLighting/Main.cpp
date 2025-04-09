#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "Common/Camera.h"
#include "Common/Context.h"
#include "Common/Mesh.h"
#include "Common/Shader.h"
#include "Common/Texture.h"
#include "Common/Window.h"

float MouseXOffset;
float MouseYOffset;
void MouseCallback(GLFWwindow* window, double xPos, double yPos);

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME);
    glfwSetCursorPosCallback(context.GetWindow().GetInternalWindow(), MouseCallback);
    glfwSetInputMode(context.GetWindow().GetInternalWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Cm::FirstPersonCamera camera;
    glm::mat4 projection(glm::perspective(glm::radians(45.0f), (float)context.GetWindow().GetWidth() / (float)context.GetWindow().GetHeight(), 0.1f, 100.0f));

    // Create opengl resources
    Cm::Shader shader({PROJECT_DIR "/Cube.frag", PROJECT_DIR "/Cube.vert"});
    Cm::Texture diffuseTexture(RESOURCE_DIR "/Prototype/texture_07.png", Cm::TextureFilter::Nearest);
    Cm::Texture smileFaceTexture(RESOURCE_DIR "/awesomeface.png", Cm::TextureFilter::Linear);

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Cm::Vertex), (void*)(offsetof(Cm::Vertex, Position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Cm::Vertex), (void*)(offsetof(Cm::Vertex, Normal))); // Not using but still must define as the Cm::Vertex includes the normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Cm::Vertex), (void*)(offsetof(Cm::Vertex, UV)));
    glBufferData(GL_ARRAY_BUFFER, Cm::Cube::VertexCount * sizeof(Cm::Vertex), Cm::Cube::Vertices, GL_STATIC_DRAW);

    std::vector cubeTransforms(Cm::Transform::GenerateMultiRandom(10));
    while (context.BeginFrame())
    {
        camera.ProcessMovement(context.GetWindow(), context.GetDeltaTime());
        camera.ProcessMouseMovement(MouseXOffset, MouseYOffset);

        // glm::vec3 camPos(sin(cameraPosition.x * cameraRadius), cameraPosition.y, cos(cameraPosition.x * cameraRadius));
        glUseProgram(shader.GetGpuId());
        // Project and View matrix
        glUniformMatrix4fv(glGetUniformLocation(shader.GetGpuId(), "u_Projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.GetGpuId(), "u_View"), 1, GL_FALSE, &camera.GetViewModel()[0][0]);

        // Textures
        glUniform1i(glGetUniformLocation(shader.GetGpuId(), "u_Diffuse"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture.GetGpuId());

        glBindVertexArray(vao);
        for (Cm::Transform& transform : cubeTransforms)
        {
            // Model matrix
            glm::mat4 model(transform.GetModel());
            glUniformMatrix4fv(glGetUniformLocation(shader.GetGpuId(), "u_Model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, Cm::Cube::VertexCount);
        }

        context.EndFrame();
        MouseXOffset = 0.0f;
        MouseYOffset = 0.0f;
    }
    return 0;
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
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
