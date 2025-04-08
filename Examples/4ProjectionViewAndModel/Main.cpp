/// Renders multiple cubes in random positions, rotations and scales
///
/// What was covered through learnopengl.com:
/// Transformations:    https://learnopengl.com/Getting-started/Transformations
/// Coordinate-System:  https://learnopengl.com/Getting-started/Coordinate-Systems
/// Camera (beginning): https://learnopengl.com/Getting-started/Camera

#include <cmath>
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "Common/Context.h"
#include "Common/Mesh.h"
#include "Common/Shader.h"
#include "Common/Texture.h"
#include "Common/Window.h"

struct Transform
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);
    glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 1.0f);
    float RotationAngle = 0.0f;

    glm::mat4 GetModel() const
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, Position);
        model = glm::scale(model, Scale);
        model = glm::rotate(model, RotationAngle, Rotation);
        return model;
    }
};

std::vector<Transform> RandomTransforms(size_t count)
{
    std::vector<Transform> transforms;

    auto inRange = [](float v, float ov) -> bool
    {
        constexpr float range = 0.5f;
        return v >= ov - range && v <= ov + range;
    };
    for (size_t i = 0; i < count; ++i)
    {
        std::random_device randomDevice;
        std::default_random_engine eng(randomDevice());
        std::uniform_real_distribution rPos(-4.0f, 4.0f);
        std::uniform_real_distribution rRot(-1.0f, 1.0f);
        std::uniform_real_distribution rSca(0.1f, 1.0f);
        Transform transform;

        bool calcPos = true;
        while (calcPos)
        {
            calcPos = false;
            transform.Position = glm::vec3(rPos(eng), rPos(eng), rPos(eng));
            for (const Transform& other : transforms)
            {
                if (inRange(transform.Position.x, other.Position.x))
                    calcPos = true;
                if (inRange(transform.Position.y, other.Position.y))
                    calcPos = true;
                if (inRange(transform.Position.z, other.Position.z))
                    calcPos = true;
            }
        }
        transform.Scale = glm::vec3(rSca(eng));
        transform.Rotation = glm::vec3(rRot(eng), rRot(eng), rRot(eng));
        transforms.push_back(transform);
    }
    return transforms;
}

void MoveCamera(Cm::Window& window, float moveSpeed, float deltaTime, glm::vec3& cameraPosition)
{
    glm::vec3 moveDirection(0.0f);
    if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_A) == GLFW_PRESS)
        moveDirection.x = -1.0f;
    if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_D) == GLFW_PRESS)
        moveDirection.x = 1.0f;
    if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        moveDirection.y = 2.0f;
    if (glfwGetKey(window.GetInternalWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        moveDirection.y = -2.0f;

    cameraPosition += moveDirection * moveSpeed * deltaTime;
}

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME);
    context.EnableImGui();

    // Enable Z-Buffer (Depth Buffer). GLFW already creates a depth buffer for you just like it does for the color buffer
    glEnable(GL_DEPTH_TEST);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Enable Z-Buffer (Depth Buffer)

    constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
    constexpr glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
    glm::mat4 projection(glm::perspective(glm::radians(45.0f), (float)context.GetWindow().GetWidth() / (float)context.GetWindow().GetHeight(), 0.1f, 100.0f));

    glm::vec3 cameraPosition(0.0f, 0.0f, -10.0f);

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


    std::vector cubeTransforms(RandomTransforms(10));
    constexpr float rotationSpeed = 0.5f;
    constexpr float cameraRadius = 10.0f;
    while (context.BeginFrame())
    {
        if (ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::SetWindowSize(ImVec2(200, 150));
            ImGui::TextWrapped("Use keys A, D to rotation around the center and Space, Control to move up and down");
            ImGui::End();
        }

        MoveCamera(context.GetWindow(), 1.0f, context.GetDeltaTime(), cameraPosition);

        // glm::vec3 camPos(sin(cameraPosition.x * cameraRadius), cameraPosition.y, cos(cameraPosition.x * cameraRadius));
        float camX = sin(cameraPosition.x) * cameraRadius;
        float camZ = cos(cameraPosition.x) * cameraRadius;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, cameraPosition.y, camZ), cameraTarget, up);

        glUseProgram(shader.GetGpuId());
        // Project and View matrix
        glUniformMatrix4fv(glGetUniformLocation(shader.GetGpuId(), "u_Projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader.GetGpuId(), "u_View"), 1, GL_FALSE, &view[0][0]);

        // Textures
        glUniform1i(glGetUniformLocation(shader.GetGpuId(), "u_Diffuse"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture.GetGpuId());

        glBindVertexArray(vao);
        for (Transform& transform : cubeTransforms)
        {
            // Model matrix
            transform.RotationAngle += rotationSpeed * context.GetDeltaTime();
            glm::mat4 model(transform.GetModel());
            glUniformMatrix4fv(glGetUniformLocation(shader.GetGpuId(), "u_Model"), 1, GL_FALSE, &model[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, Cm::Cube::VertexCount);
        }

        context.EndFrame();
    }
    return 0;
}
