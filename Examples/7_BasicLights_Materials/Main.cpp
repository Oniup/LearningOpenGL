#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <iostream>
#include <random>

#include "Common/Camera.h"
#include "Common/Color.h"
#include "Common/Context.h"
#include "Common/Mesh.h"
#include "Common/Shader.h"
#include "Common/Texture.h"
#include "Common/VertexBuffer.h"
#include "Common/Window.h"

static constexpr size_t MaxLightCount = 10;

struct Light
{
    Cm::Transform Transform;
    glm::vec3 Color = glm::vec3(1.0f);

    float Intencity;
    float Linear;
    float Quadratic;
};

struct Material
{
    Cm::Texture* DiffuseMap;
    Cm::Texture* SpecularMap;
    Cm::Texture* EmissionMap;
    float Shininess;
};

struct Cube
{
    Cm::Transform Transform;
    Material Material;
};

void CalcLightIntencity(Light& light)
{
    const float a = 0.640f;
    const float b = 0.130f;
    const float c = 83.300f;
    light.Linear = a / (light.Intencity * b);
    light.Quadratic = c / (light.Intencity * light.Intencity);
}

void AddLight(std::vector<Light>& lights)
{
    Light light;
    light.Transform.Position = glm::vec3(0.0f);
    light.Transform.Rotation = glm::vec3(0.0f);
    light.Transform.Scale = glm::vec3(0.1f);
    light.Intencity = 50.0f;
    CalcLightIntencity(light);
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
    Cm::Texture containerDiffuseMap(RESOURCE_DIR "/Container/Diffuse.png", Cm::TextureFilter::Linear);
    Cm::Texture containerSpecularMap(RESOURCE_DIR "/Container/Specular.png", Cm::TextureFilter::Linear);
    Cm::Texture containerEmissionMap(RESOURCE_DIR "/Container/Emission.jpg", Cm::TextureFilter::Linear);

    Cm::VertexBuffer cubeVertexBuffer;
    cubeVertexBuffer.PushData(Cm::Cube::VertexCount, Cm::Cube::Vertices);

    Cm::Shader lightShader({ PROJECT_DIR "/Default.vert", PROJECT_DIR "/Light.frag" });
    std::vector<Light> lights;
    AddLight(lights);
    lights.front().Transform.Position = glm::vec3(0.0f, 0.0f, -5.0f);

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

    std::vector<Cube> scene;
    scene.push_back(Cube{});
    scene.front().Material.DiffuseMap = &containerDiffuseMap;
    scene.front().Material.SpecularMap = &containerSpecularMap;
    scene.front().Material.EmissionMap = &containerEmissionMap;
    scene.front().Material.Shininess = 32.0f;
    bool enableEmissionMap = false;
    while (context.BeginFrame())
    {
        if (ImGui::Begin("Options"))
        {
            if (ImGui::Checkbox("Emission Map", &enableEmissionMap))
            {
                for (Cube& cube : scene)
                    cube.Material.EmissionMap = enableEmissionMap ? &containerEmissionMap : nullptr;
            }
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
                        ImGui::DragFloat("Intencity", &lights[i].Intencity, 1.0f, 0.1f);
                        CalcLightIntencity(lights[i]);
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

        cubeVertexBuffer.Bind();
        cubeShader.UniformF3("u_AmbientLight", context.GetClearColor());
        cubeShader.UniformI("u_LightCount", lights.size());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            constexpr size_t NameSize = 256;
            char name[NameSize];
            snprintf(name, NameSize, "u_Lights[%zu].Position", i);
            cubeShader.UniformF3(name, lights[i].Transform.Position);
            snprintf(name, NameSize, "u_Lights[%zu].Color", i);
            cubeShader.UniformF3(name, lights[i].Color);
            snprintf(name, NameSize, "u_Lights[%zu].Linear", i);
            cubeShader.UniformF(name, lights[i].Linear);
            snprintf(name, NameSize, "u_Lights[%zu].Quadratic", i);
            cubeShader.UniformF(name, lights[i].Quadratic);
        }

        for (const Cube& cube : scene)
        {
            cubeShader.UniformI("u_Material.DiffuseMap", 0);
            cubeShader.UniformI("u_Material.SpecularMap", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cube.Material.DiffuseMap->GetGpuId());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, cube.Material.SpecularMap->GetGpuId());

            cubeShader.UniformI("u_Material.EnableEmissionMap", (int)enableEmissionMap);
            if (cube.Material.EmissionMap)
            {
                cubeShader.UniformI("u_Material.EmissionMap", 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, cube.Material.EmissionMap->GetGpuId());
            }

            cubeShader.UniformF("u_Material.Shininess", cube.Material.Shininess);

            cubeShader.UniformF3("u_ViewPosition", camera.Position);
            cubeShader.UniformMat4("u_Model", cube.Transform.GetModel());
            cubeVertexBuffer.Draw(Cm::DrawMode::Triangles);
            glDrawArrays(GL_TRIANGLES, 0, Cm::Cube::VertexCount);
        }

        lightShader.Bind();
        for (const Light& light : lights)
        {
            lightShader.UniformMat4("u_Model", light.Transform.GetModel());
            lightShader.UniformF3("u_LightColor", light.Color);
            glDrawArrays(GL_TRIANGLES, 0, Cm::Cube::VertexCount);
            cubeVertexBuffer.Draw(Cm::DrawMode::Triangles);
        }

        context.EndFrame();
    }
    return 0;
}
