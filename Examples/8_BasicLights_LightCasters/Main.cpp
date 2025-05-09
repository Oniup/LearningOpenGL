#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "Common/Camera.h"
#include "Common/Context.h"
#include "Common/Shader.h"
#include "Common/Texture.h"
#include "Common/Vertex.h"
#include "Common/VertexBuffer.h"
#include "Common/Window.h"

#define MAX_LIGHT_COUNT_DIRECTIONAL 4
#define MAX_LIGHT_COUNT_POINT 10
#define MAX_LIGHT_COUNT_SPOT 10

struct alignas(16) DirectionalLight
{
    alignas(16) glm::vec3 Direction;
    alignas(16) glm::vec3 Color;
    alignas(16) glm::vec3 AmbientColor;
    float Intensity;
};

struct alignas(16) PointLight
{
    alignas(16) glm::vec3 Position;
    alignas(16) glm::vec3 Color;
    float Intensity;
    float Linear;
    float Quadratic;
};

struct alignas(16) SpotLight
{
    alignas(16) glm::vec3 Position;
    alignas(16) glm::vec3 Direction;
    alignas(16) glm::vec3 Color;
    float Intensity;
    float Linear;
    float Quadratic;
};

struct Lights
{
    unsigned int DirectionalCount;
    unsigned int PointCount;
    unsigned int SpotCount;
    DirectionalLight Directionals[MAX_LIGHT_COUNT_DIRECTIONAL];
    PointLight Points[MAX_LIGHT_COUNT_POINT];
    SpotLight Spots[MAX_LIGHT_COUNT_SPOT];
    unsigned int UniformBuffer;

    void Create();
    void BindUniformBufferToIndex(unsigned int index);
    void RenderLights(Cm::VertexBuffer& vertexBuffer, Cm::Shader& shader);
    void SetImGuiOptions();

    constexpr static size_t UniformBufferSize()
    {
        size_t dSize = sizeof(DirectionalLight) * MAX_LIGHT_COUNT_DIRECTIONAL;
        size_t pSize = sizeof(PointLight) * MAX_LIGHT_COUNT_POINT;
        size_t sSize = sizeof(SpotLight) * MAX_LIGHT_COUNT_SPOT;
        return sizeof(unsigned int) * 3 + dSize + pSize + sSize;
    }

    template <typename TLight>
    static void SetDistance(TLight& light, float distance)
    {
        constexpr float a = 0.640f;
        constexpr float b = 0.130f;
        constexpr float c = 83.300f;
        light.Linear = a / (distance * b);
        light.Quadratic = c / (distance * distance);
    }

    template <typename TLight>
    static float GetDistance(TLight& light)
    {
        constexpr float c = 83.300f;
        return sqrt(c / light.Quadratic);
    }
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

void RenderScene(const std::vector<Cube>& scene, Cm::Shader& shader, Cm::VertexBuffer& vertexBuffer, const Cm::Camera& camera, bool enableEmissionMap, const Cm::Context& context);
void CreateScene(std::vector<Cube>& scene, Cm::Texture* diffuseMap, Cm::Texture* specularMap, Cm::Texture* emissionMap);

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME, Cm::WindowFlags_Borderless);
    glEnable(GL_DEPTH_TEST);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    context.EnableImGui();

    float greyColor = 22.0f / 255.0f;
    context.GetClearColor() = glm::vec4(greyColor);
    context.GetClearColor().a = 1.0f;

    Cm::FirstPersonCamera camera(context);
    camera.MoveSpeed = 12.0f;
    camera.Position = glm::vec3(0.0f, 0.0f, -10.0f);

    // Create opengl resources
    Cm::Shader cubeShader({PROJECT_DIR "/Cube.frag", PROJECT_DIR "/Default.vert"});
    Cm::Texture containerDiffuseMap(RESOURCE_DIR "/Container/Diffuse.png", Cm::TextureFilter::Linear);
    Cm::Texture containerSpecularMap(RESOURCE_DIR "/Container/Specular.png", Cm::TextureFilter::Linear);
    Cm::Texture containerEmissionMap(RESOURCE_DIR "/Container/Emission.jpg", Cm::TextureFilter::Linear);

    Cm::Texture floorDiffuseMap(RESOURCE_DIR "/Textures/StylizedWoodPlanks/baseColor.png", Cm::TextureFilter::Linear);
    Cm::Texture floorSpecularMap(RESOURCE_DIR "/Textures/StylizedWoodPlanks/metallic.png", Cm::TextureFilter::Linear);

    Cm::VertexBuffer cubeVertexBuffer;
    cubeVertexBuffer.PushData(Cm::Cube::VertexCount, Cm::Cube::Vertices);

    // Create lights
    Cm::Shader lightShader({ PROJECT_DIR "/Default.vert", PROJECT_DIR "/Light.frag" });
    Lights lights;
    lights.Create();

    // Create scene
    std::vector<Cube> scene;
    CreateScene(scene, &containerDiffuseMap, &containerSpecularMap, &containerEmissionMap);

    scene.push_back({});
    Cube& floor = scene.back();
    floor.Transform.Position.y = -5;
    floor.Transform.Scale = glm::vec3(5.0f, 0.5f, 5.0f);
    floor.Material = Material{ &floorDiffuseMap, &floorSpecularMap, nullptr, 32 };

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

    // Set the projection matrix once
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetProjectionMatrix(context)[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
            ImGui::ColorEdit3("Clear Color", &context.GetClearColor()[0]);
            if (ImGui::CollapsingHeader("Camera"))
            {
                float lastFov = camera.Fov;
                ImGui::DragFloat("FOV", &camera.Fov);
                ImGui::DragFloat("Mouse Sensitivity", &camera.MouseSensitivity, 0.01f);

                camera.Fov = std::clamp(camera.Fov, 5.0f, 90.0f);
                camera.MouseSensitivity = std::clamp(camera.MouseSensitivity, 0.0f, 2.0f);
                if (lastFov != camera.Fov)
                {
                    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
                    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetProjectionMatrix(context)[0][0]);
                    glBindBuffer(GL_UNIFORM_BUFFER, 0);
                }
            }
            lights.SetImGuiOptions();
            ImGui::End();
        }

        camera.ProcessMovement(context.GetWindow(), context.GetDeltaTime());
        camera.ProcessMouseMovement();

        // Set the view matrix every frame
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        lights.BindUniformBufferToIndex(1);
        lights.RenderLights(cubeVertexBuffer, lightShader);
        RenderScene(scene, cubeShader, cubeVertexBuffer, camera, enableEmissionMap, context);

        context.EndFrame();
    }
    return 0;
}

void Lights::Create()
{
    glGenBuffers(1, &UniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, UniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, UniformBufferSize(), nullptr, GL_DYNAMIC_DRAW);

    DirectionalCount = 0;
    PointCount = 0;
    SpotCount = 0;
    // Points[0].Color = glm::vec3(1.0f);
    // Points[0].Position = glm::vec3(0.0f, 0.0f, -5.0f);
    // Points[0].Intensity = 1.0f;
    // SetDistance(Points[0], 50.0f);
}

void Lights::BindUniformBufferToIndex(unsigned int index)
{
    glBindBuffer(GL_UNIFORM_BUFFER, UniformBuffer);

    size_t pOffset = sizeof(DirectionalLight) * MAX_LIGHT_COUNT_DIRECTIONAL;
    size_t sOffset = pOffset + sizeof(PointLight) * MAX_LIGHT_COUNT_POINT;
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirectionalLight) * MAX_LIGHT_COUNT_DIRECTIONAL, Directionals);
    glBufferSubData(GL_UNIFORM_BUFFER, pOffset, sizeof(PointLight) * MAX_LIGHT_COUNT_POINT, Points);
    glBufferSubData(GL_UNIFORM_BUFFER, sOffset, sizeof(SpotLight) * MAX_LIGHT_COUNT_SPOT, Spots);

    size_t cOffset = sOffset + sizeof(SpotLight) * MAX_LIGHT_COUNT_SPOT;
    glBufferSubData(GL_UNIFORM_BUFFER, cOffset + 0 * sizeof(unsigned int), sizeof(unsigned int), &DirectionalCount);
    glBufferSubData(GL_UNIFORM_BUFFER, cOffset + 1 * sizeof(unsigned int), sizeof(unsigned int), &PointCount);
    glBufferSubData(GL_UNIFORM_BUFFER, cOffset + 2 * sizeof(unsigned int), sizeof(unsigned int), &SpotCount);

    glBindBufferRange(GL_UNIFORM_BUFFER, index, UniformBuffer, 0, UniformBufferSize());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::RenderLights(Cm::VertexBuffer& vertexBuffer, Cm::Shader& shader)
{
    Cm::Transform transform;
    transform.Scale = glm::vec3(0.05f);
    transform.Rotation = glm::vec3(0.0f);

    shader.Bind();
    for (size_t i = 0; i < PointCount; ++i)
    {
        PointLight& light = Points[i];
        transform.Position = light.Position;
        shader.UniformMat4("u_Model", transform.GetModel());
        shader.UniformF3("u_LightColor", light.Color);
        vertexBuffer.Draw(Cm::DrawMode::Triangles);
    }
    for (size_t i = 0; i < SpotCount; ++i)
    {
        SpotLight& light = Spots[i];
        transform.Position = light.Position;
        shader.UniformMat4("u_Model", transform.GetModel());
        shader.UniformF3("u_LightColor", light.Color);
        vertexBuffer.Draw(Cm::DrawMode::Triangles);
    }
}

void Lights::SetImGuiOptions()
{
    if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static float snap = 0.2f;
        ImGui::DragFloat("Drag Value Speed", &snap, 0.1f);
        if (ImGui::TreeNodeEx("Directional", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (DirectionalCount < MAX_LIGHT_COUNT_DIRECTIONAL && ImGui::Button("Add"))
            {
                DirectionalLight& light = Directionals[DirectionalCount];
                light.Direction = glm::vec3(0.0f, -1.0f, 0.5f);
                light.Color = glm::vec3(1.0f);
                light.AmbientColor = glm::vec3(0.1f);
                light.Intensity = 1.0f;
                DirectionalCount++;
            }
            for (size_t i = 0; i < DirectionalCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx((std::string("Directional Light ") + std::to_string(i + 1)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    DirectionalLight& light = Directionals[i];
                    ImGui::DragFloat3("Direction", &light.Direction[0], 0.05f, -1.0f, 1.0f);
                    ImGui::ColorEdit3("Color", &light.Color[0]);
                    ImGui::ColorEdit3("Ambient Color", &light.AmbientColor[0]);
                    ImGui::DragFloat("Intensity", &light.Intensity, 0.05f);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
        if (ImGui::TreeNodeEx("Point", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (PointCount < MAX_LIGHT_COUNT_POINT && ImGui::Button("Add"))
            {
                PointLight& light = Points[PointCount];
                light.Position = glm::vec3(0.0f);
                light.Color = glm::vec3(1.0f);
                light.Intensity = 1.0f;
                SetDistance(light, 50.0f);
                PointCount++;
            }
            for (size_t i = 0; i < PointCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx((std::string("Point Light ") + std::to_string(i + 1)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    PointLight& light = Points[i];
                    ImGui::DragFloat3("Position", &light.Position[0], snap);
                    ImGui::ColorEdit3("Color", &light.Color[0]);
                    ImGui::DragFloat("Intensity", &light.Intensity, 0.05f);
                    float distance = GetDistance(light);
                    float lastDistance = distance;
                    ImGui::DragFloat("Distance", &distance);
                    if (lastDistance != distance)
                        SetDistance(light, distance);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }
}

void RenderScene(const std::vector<Cube>& scene, Cm::Shader& shader, Cm::VertexBuffer& vertexBuffer, const Cm::Camera& camera, bool enableEmissionMap, const Cm::Context& context)
{
    shader.Bind();
    vertexBuffer.Bind();
    // shader.UniformF3("u_AmbientLight", context.GetClearColor());

    for (const Cube& cube : scene)
    {
        shader.UniformI("u_Material.DiffuseMap", 0);
        shader.UniformI("u_Material.SpecularMap", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.Material.DiffuseMap->GetGpuId());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube.Material.SpecularMap->GetGpuId());

        if (cube.Material.EmissionMap)
        {
            shader.UniformI("u_Material.EnableEmissionMap", enableEmissionMap);
            shader.UniformI("u_Material.EmissionMap", 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cube.Material.EmissionMap->GetGpuId());
        }
        else
            shader.UniformI("u_Material.EnableEmissionMap", false);

        shader.UniformF("u_Material.Shininess", cube.Material.Shininess);

        shader.UniformF3("u_ViewPosition", camera.Position);
        shader.UniformMat4("u_Model", cube.Transform.GetModel());
        vertexBuffer.Draw(Cm::DrawMode::Triangles);
    }
}
void CreateScene(std::vector<Cube>& scene, Cm::Texture* diffuseMap, Cm::Texture* specularMap, Cm::Texture* emissionMap)
{
    scene.resize(10);
    for (Cube& cube : scene)
    {
        cube.Transform.GenerateRandom();
        cube.Material.DiffuseMap = diffuseMap;
        cube.Material.SpecularMap = specularMap;
        cube.Material.EmissionMap = emissionMap;
        cube.Material.Shininess = 32.0f;
    }
}
