#include <glad/glad.h>
#include <imgui/imgui.h>

#include "Common/Camera.h"
#include "Common/Context.h"
#include "Common/Lights.h"
#include "Common/Mesh.h"
#include "Common/Shader.h"

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME);
    context.EnableImGui();
    context.GetClearColor() = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Cm::LightsManager lights;
    lights.UpdateGpuBuffer();

    Cm::Shader shader({ PROJECT_DIR "/Model.frag", PROJECT_DIR "/Model.vert" });
    Cm::Shader lightShader({ PROJECT_DIR "/Light.frag", PROJECT_DIR "/Model.vert" });
    Cm::Model model(RESOURCE_DIR "/Meshes/backpack/backpack.obj");
    Cm::Model sphere(RESOURCE_DIR "/Meshes/Sphere/Sphere.obj");

    Cm::FirstPersonCamera camera(context);
    camera.Position.z = -10;

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, &camera.GetProjectionMatrix(context)[0][0], GL_STATIC_DRAW);

    Cm::Transform transform;
    transform.Position = glm::vec3(0.0f);

    while (context.BeginFrame())
    {
        if (ImGui::Begin("Options"))
        {
            ImGui::ColorEdit3("Clear Color", &context.GetClearColor()[0]);
            if (ImGui::CollapsingHeader("Camera"))
            {
                float lastFov = camera.Fov;
                ImGui::DragFloat("FOV", &camera.Fov);
                ImGui::DragFloat("Mouse Sensitivity", &camera.MouseSensitivity, 0.01f);

                // camera.Fov = std::clamp(camera.Fov, 5.0f, 90.0f);
                camera.MouseSensitivity = std::clamp(camera.MouseSensitivity, 0.0f, 2.0f);
                if (lastFov != camera.Fov)
                {
                    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
                    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &camera.GetProjectionMatrix(context)[0][0]);
                    glBindBuffer(GL_UNIFORM_BUFFER, 0);
                }
            }
            lights.ImGuiEdit();
            ImGui::End();
        }

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(glm::mat4) * 2);
        lights.Bind(1);

        camera.ProcessMouseMovement();
        camera.ProcessMovement(context.GetWindow(), context.GetDeltaTime());

        shader.Bind();
        shader.UniformMat4("u_Model", transform.GetModel());
        shader.UniformF3("u_ViewPosition", camera.Position);
        model.Draw(shader);

        lightShader.Bind();
        for (unsigned int i = 0; i < lights.GetPointLightCount(); i++)
        {
            Cm::Transform transform;
            transform.Scale = glm::vec3(0.2f);
            transform.Position = lights.GetPointLights()[i].Position;
            lightShader.UniformMat4("u_Model", transform.GetModel());
            lightShader.UniformF3("u_LightBaseColor", lights.GetPointLights()[i].Color.Base);
            sphere.Draw(lightShader);
        }
        for (unsigned int i = 0; i < lights.GetSpotLightsCount(); i++)
        {
            Cm::Transform transform;
            transform.Scale = glm::vec3(0.2f);
            transform.Position = lights.GetSpotLights()[i].Position;
            lightShader.UniformMat4("u_Model", transform.GetModel());
            lightShader.UniformF3("u_LightBaseColor", lights.GetSpotLights()[i].Color.Base);
            sphere.Draw(lightShader);
        }

        context.EndFrame();
    }
    return 0;
}
