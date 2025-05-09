#include <random>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "Common/Camera.h"
#include "Common/Color.h"
#include "Common/Context.h"
#include "Common/Shader.h"
#include "Common/Texture.h"
#include "Common/Vertex.h"
#include "Common/VertexBuffer.h"
#include "Common/Window.h"

static constexpr size_t MaxLightCount = 10;

struct Light
{
    Cm::Transform Transform;
    glm::vec3 Color = glm::vec3(1.0f);

    float Intensity;
    float Linear;
    float Quadratic;
};

void CalcLightIntensity(Light& light)
{
    constexpr float a = 0.640f;
    constexpr float b = 0.130f;
    constexpr float c = 83.300f;
    light.Linear = a / (light.Intensity * b);
    light.Quadratic = c / (light.Intensity * light.Intensity);
}

void AddLight(std::vector<Light>& lights)
{
    Light light;
    light.Transform.Position = glm::vec3(0.0f);
    light.Transform.Rotation = glm::vec3(0.0f);
    light.Transform.Scale = glm::vec3(0.1f);
    light.Intensity = 50.0f;
    CalcLightIntensity(light);
    lights.push_back(std::move(light));
}

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME);
    glEnable(GL_DEPTH_TEST);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    context.EnableImGui();

    float greyColor = 22.0f / 255.0f;
    context.GetClearColor() = glm::vec4(greyColor);
    context.GetClearColor().a = 1.0f;

    Cm::FirstPersonCamera camera(context);
    camera.Position = glm::vec3(0.0f, 0.0f, -10.0f);

    // Create opengl resources
    Cm::Shader cubeShader({PROJECT_DIR "/Cube.frag", PROJECT_DIR "/Default.vert"});
    Cm::Texture diffuseTexture(RESOURCE_DIR "/Prototype/texture_07.png", Cm::TextureFilter::Nearest);
    Cm::Texture smileFaceTexture(RESOURCE_DIR "/awesomeface.png", Cm::TextureFilter::Linear);

    Cm::VertexBuffer cubeVertexBuffer;
    cubeVertexBuffer.PushData(Cm::Cube::VertexCount, Cm::Cube::Vertices);

    Cm::Shader lightShader({ PROJECT_DIR "/Default.vert", PROJECT_DIR "/Light.frag" });
    std::vector<Light> lights;
    AddLight(lights);

    // Setting the index of the 'Matrices' uniform block struct in the shader so that all shaders can share the same data.
    glUniformBlockBinding(cubeShader.GetGpuId(), glGetUniformBlockIndex(cubeShader.GetGpuId(), "Matrices"), 0);
    glUniformBlockBinding(lightShader.GetGpuId(), glGetUniformBlockIndex(lightShader.GetGpuId(), "Matrices"), 0);

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    // Set the projection matrix once
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)context.GetWindow().GetWidth() / (float)context.GetWindow().GetHeight(), 0.1f, 100.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    std::vector cubeTransforms(Cm::Transform::GenerateMultiRandom(10));
    while (context.BeginFrame())
    {
        if (ImGui::Begin("Light"))
        {
            ImGui::ColorEdit3("Ambient Light", &context.GetClearColor()[0]);
            if (ImGui::TreeNodeEx("Light Objects", ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (size_t i = 0; i < lights.size(); ++i)
                {
                    ImGui::PushID(i);
                    char name[50];
                    snprintf(name, 50, "Light %zu", i + 1);
                    if (ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
                    {
                        ImGui::DragFloat3("Position", &lights[i].Transform.Position[0], 0.05f);
                        ImGui::ColorEdit3("Color", &lights[i].Color[0]);
                        ImGui::DragFloat("Intencity", &lights[i].Intensity, 1.0f, 0.1f);
                        CalcLightIntensity(lights[i]);
                        ImGui::Text("Linear: %f, Quadratic: %f", lights[i].Linear, lights[i].Quadratic);
                    }
                    ImGui::PopID();
                }
                if (lights.size() < MaxLightCount)
                {
                    if (ImGui::Button("Add"))
                        AddLight(lights);
                }
                ImGui::TreePop();
            }
            ImGui::End();
        }

        camera.ProcessMovement(context.GetWindow(), context.GetDeltaTime());
        camera.ProcessMouseMovement();

        // Set the view matrix every frame
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        cubeShader.Bind();

        //// Textures
        cubeShader.UniformI("u_Diffuse", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture.GetGpuId());

        cubeVertexBuffer.Bind();
        cubeShader.UniformF3("u_AmbientLight", context.GetClearColor());
        cubeShader.UniformI("u_LightCount", lights.size());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            constexpr size_t nameSize = 256;
            char name[nameSize];
            snprintf(name, nameSize, "u_Lights[%zu].Position", i);
            cubeShader.UniformF3(name, lights[i].Transform.Position);
            snprintf(name, nameSize, "u_Lights[%zu].Color", i);
            cubeShader.UniformF3(name, lights[i].Color);
            snprintf(name, nameSize, "u_Lights[%zu].Linear", i);
            cubeShader.UniformF(name, lights[i].Linear);
            snprintf(name, nameSize, "u_Lights[%zu].Quadratic", i);
            cubeShader.UniformF(name, lights[i].Quadratic);
        }

        for (const Cm::Transform& transform : cubeTransforms)
        {
            cubeShader.UniformF3("u_ViewPosition", camera.Position);
            cubeShader.UniformMat4("u_Model", transform.GetModel());
            cubeVertexBuffer.Draw(Cm::DrawMode::Triangles);
        }

        lightShader.Bind();
        for (const Light& light : lights)
        {
            lightShader.UniformMat4("u_Model", light.Transform.GetModel());
            lightShader.UniformF3("u_LightColor", light.Color);
            cubeVertexBuffer.Draw(Cm::DrawMode::Triangles);
        }

        context.EndFrame();
    }
    return 0;
}
