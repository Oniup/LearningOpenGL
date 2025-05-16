#include <glad/glad.h>
#include <imgui/imgui.h>

#include "Common/Camera.h"
#include "Common/Context.h"
#include "Common/Lights.h"
#include "Common/Mesh.h"
#include "Common/Shader.h"

struct SceneObject
{
    Cm::Transform Transform;
    Cm::Model* Model;
};

void main()
{
    Cm::Context context(PROJECT_NAME);
    context.EnableImGui();
    context.GetClearColor() = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    Cm::LightsManager lights;
    Cm::Shader lightShader(Cm::LightsManager::CreateLightObjectShader());
    Cm::Model lightObject(RESOURCE_DIR "/Meshes/Sphere/Sphere.obj");
    lights.AddDirectionalLight();
    lights.GetDirectionalLights()[0].Direction = glm::vec3(0.2, -1.0f, 0.8f);
    lights.GetDirectionalLights()[0].Color.Base = glm::vec3(0.711f, 0.823f, 0.851f);
    lights.GetDirectionalLights()[0].Color.Specular = glm::vec3(0.483f, 0.728f, 0.785f);
    lights.UpdateGpuBuffer();

    Cm::Shader shader({ PROJECT_DIR "/Model.frag", PROJECT_DIR "/Model.vert" });
    Cm::Shader depthShader({ PROJECT_DIR "/Depth.frag", PROJECT_DIR "/Model.vert" });
    Cm::Model model(RESOURCE_DIR "/Meshes/backpack/backpack.obj");

    Cm::Texture diffuseMap(RESOURCE_DIR "/Container/Diffuse.png", Cm::TextureFilter::Linear);
    Cm::Texture specularMap(RESOURCE_DIR "/Container/Specular.png", Cm::TextureFilter::Linear);
    Cm::Model cube(Cm::Mesh(Cm::Cube::VertexCount, Cm::Cube::Vertices, 0, nullptr, { {&diffuseMap, aiTextureType_DIFFUSE}, {&specularMap, aiTextureType_SPECULAR} }));

    Cm::FirstPersonCamera camera(context);
    camera.Position.z = -10;

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, &camera.GetProjectionMatrix(context)[0][0], GL_STATIC_DRAW);

    std::vector<SceneObject> scene;
    // Creates
    {
        std::vector<Cm::Transform> crateTransforms = {
            Cm::Transform{glm::vec3(5.0f, 0.0f, 0.0f)},
            Cm::Transform{glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(10.0f, 0.2f, 10.0f)},
        };
        for (const Cm::Transform& transform : crateTransforms)
        {
            scene.push_back({ transform, &cube });
            scene.back().Model->SetAllMeshesTextures(Cm::MeshTexture{ &diffuseMap, aiTextureType_DIFFUSE });
            scene.back().Model->SetAllMeshesTextures(Cm::MeshTexture{ &specularMap, aiTextureType_SPECULAR });
        }
    }
    // Backpack
    {
        Cm::Transform modelTransform;
        modelTransform.Position.z = 5.0f;
        modelTransform.Rotation.y = 180.0f;
        scene.push_back({ modelTransform, &model });
    }

    int lastWinSize = 0;
    int depthMode = 1;
    int activeShader = 0;
    Cm::Shader* activeShaderPointer = &shader;
    while (context.BeginFrame())
    {
        int winSize = context.GetWindow().GetWidth() + context.GetWindow().GetHeight();
        float lastFov = camera.Fov;
        if (ImGui::Begin("Options"))
        {
            constexpr const char* activeShaderNames[] = {
                "Base",
                "Depth",
            };
            if (ImGui::Combo("Active Shader", &activeShader, activeShaderNames, std::size(activeShaderNames)))
            {
                switch (activeShader)
                {
                case 0:
                    activeShaderPointer = &shader;
                    break;
                case 1:
                    activeShaderPointer = &depthShader;
                    break;
                }
            }
            // Depth
            if (ImGui::CollapsingHeader("Depth Modes"))
            {
                constexpr const char* depthModeNames[] = {
                    "GL_NEVER",
                    "GL_LESS",
                    "GL_EQUAL",
                    "GL_LEQUAL",
                    "GL_GREATER",
                    "GL_NOTEQUAL",
                    "GL_GEQUAL",
                    "GL_ALWAYS",
                };
                if (ImGui::Combo("Depth Mode", &depthMode, depthModeNames, std::size(depthModeNames)))
                    glDepthFunc(GL_NEVER + depthMode);
                ImGui::Separator();
            }

            // Other
            ImGui::ColorEdit3("Clear Color", &context.GetClearColor()[0]);
            if (ImGui::CollapsingHeader("Camera"))
            {
                ImGui::DragFloat("FOV", &camera.Fov, 5.0f, 90.0f);
                ImGui::DragFloat("Mouse Sensitivity", &camera.MouseSensitivity, 0.01f, 0.1, 2.0f);
            }
            if (lights.ImGuiEdit())
                lights.UpdateGpuBuffer();
            ImGui::End();
        }

        camera.ProcessMovement(context);
        camera.ProcessMouseMovement();

        if (lastFov != camera.Fov || lastWinSize != winSize)
        {
            glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetProjectionMatrix(context)[0][0]);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(glm::mat4) * 2);
        lights.Bind(1);

        glm::vec3 offset(0.0f);
        activeShaderPointer->Bind();
        int i = 0;
        for (SceneObject& object : scene)
        {
            activeShaderPointer->UniformMat4("u_Model", object.Transform.GetModelMatrix());
            activeShaderPointer->UniformF3("u_ViewPosition", camera.Position);
            object.Model->Draw(shader);
            ++i;
        }

        lights.DrawLightObjects(lightShader, lightObject, lightObject);
        context.EndFrame();
        lastWinSize = winSize;
    }
}
