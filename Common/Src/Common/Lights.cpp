#include "Common/Lights.h"

#include <algorithm>
#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Cm
{
    void LightColor::SetToDefault()
    {
        Base = glm::vec3(1.0f);
        Specular = glm::vec3(1.0f);
        Ambient = glm::vec3(0.09f);
        Intensity = 1.0f;
    }

    void LightAttenuation::SetToDefault()
    {
        SetDistance(50.0f);
    }

    float LightAttenuation::GetDistance() const
    {
        constexpr float a = 0.640f;
        constexpr float b = 0.130f;
        constexpr float c = 83.300f;

        float fromLinear = a / (Linear * b);
        return fromLinear;
    }

    void LightAttenuation::SetDistance(float distance)
    {
        constexpr float a = 0.640f;
        constexpr float b = 0.130f;
        constexpr float c = 83.300f;
        Linear = a / (distance * b);
        Quadratic = c / (distance * distance);
    }

    LightsManager::LightsManager()
        : m_GpuBuffer(std::numeric_limits<unsigned int>::max()), m_PointLightsCount(0), m_DirectionalLightsCount(0), m_SpotLightsCount(0)
    {
        CreateGpuBuffer();
    }

    LightsManager::~LightsManager()
    {
        if (m_GpuBuffer != std::numeric_limits<unsigned int>::max())
        {
            glDeleteBuffers(1, &m_GpuBuffer);
            m_GpuBuffer = std::numeric_limits<unsigned int>::max();
        }
    }

    bool LightsManager::ImGuiEdit()
    {
        int update = 0;
        if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("Drag Value Speed", &m_ImGuiDragSpeed, 0.1f);
            update += DirectionalLightEdit();
            ImGui::Separator();
            update += PointLightEdit();
            ImGui::Separator();
            update += SpotLightEdit();
        }
        return update > 0;
    }

    void LightsManager::Bind(unsigned int uboBindingIndex)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, uboBindingIndex, m_GpuBuffer, 0, GpuBufferSize());
    }

    bool LightsManager::AddPointLight()
    {
        if (m_PointLightsCount < MaxPointLightCount)
        {
            PointLight& light = m_PointLights[m_PointLightsCount];
            ++m_PointLightsCount;

            light.Position = glm::vec3(0.0f);
            light.Color.SetToDefault();
            light.Attenuation.SetDistance(50.0f);
            return true;
        }
        return false;
    }

    bool LightsManager::AddDirectionalLight()
    {
        if (m_DirectionalLightsCount < MaxDirectionalLightCount)
        {
            DirectionalLight& light = m_DirectionalLights[m_DirectionalLightsCount];
            ++m_DirectionalLightsCount;

            light.Direction = glm::vec3(0.2, -1.0f, -1.0f);
            light.Color.SetToDefault();
            return true;
        }
        return false;
    }

    bool LightsManager::AddSpotLight()
    {
        if (m_SpotLightsCount < MaxSpotLightCount)
        {
            SpotLight& light = m_SpotLights[m_SpotLightsCount];
            ++m_SpotLightsCount;

            light.Position = glm::vec3(0.0f);
            light.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
            light.Color.SetToDefault();
            light.Attenuation.SetToDefault();
            light.CutOff = 12.5f;
            light.OuterCutOff = 17.5f;
            return true;
        }
        return false;
    }

    bool LightsManager::RemovePointLight(unsigned int index)
    {
        if (index < m_PointLightsCount)
        {
            if (index != m_PointLightsCount - 1)
                std::copy(m_PointLights + index + 1, m_PointLights + m_PointLightsCount, m_PointLights + index);
            --m_PointLightsCount;
            return true;
        }
        return false;
    }

    bool LightsManager::RemoveDirectionalLight(unsigned int index)
    {
        if (index < m_DirectionalLightsCount)
        {
            if (index != m_DirectionalLightsCount - 1)
                std::copy(m_DirectionalLights + index + 1, m_DirectionalLights + m_DirectionalLightsCount, m_DirectionalLights + index);
            --m_DirectionalLightsCount;
            return true;
        }
        return false;
    }

    bool LightsManager::RemoveSpotLight(unsigned int index)
    {
        if (index < m_SpotLightsCount)
        {
            if (index != m_SpotLightsCount - 1)
                std::copy(m_SpotLights + index + 1, m_SpotLights + m_SpotLightsCount, m_SpotLights + index);
            --m_SpotLightsCount;
            return true;
        }
        return false;
    }

    void LightsManager::UpdateGpuBuffer()
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_GpuBuffer);

        // Light buffers
        unsigned int directionalLightsOffset = sizeof(PointLight) * MaxPointLightCount;
        unsigned int spotLightOffset = directionalLightsOffset + sizeof(DirectionalLight) * MaxDirectionalLightCount;
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PointLight) * MaxPointLightCount, m_PointLights);
        glBufferSubData(GL_UNIFORM_BUFFER, directionalLightsOffset, sizeof(DirectionalLight) * MaxDirectionalLightCount, m_DirectionalLights);
        glBufferSubData(GL_UNIFORM_BUFFER, spotLightOffset, sizeof(SpotLight) * MaxSpotLightCount, m_SpotLights);

        // Count buffers
        unsigned int countOffset = spotLightOffset + sizeof(SpotLight) * MaxSpotLightCount;
        //unsigned int countOffset = directionalLightsOffset + sizeof(DirectionalLight) * MaxDirectionalLightCount;
        glBufferSubData(GL_UNIFORM_BUFFER, countOffset + 0 * sizeof(unsigned int), sizeof(unsigned int), &m_PointLightsCount);
        glBufferSubData(GL_UNIFORM_BUFFER, countOffset + 1 * sizeof(unsigned int), sizeof(unsigned int), &m_DirectionalLightsCount);
        glBufferSubData(GL_UNIFORM_BUFFER, countOffset + 2 * sizeof(unsigned int), sizeof(unsigned int), &m_SpotLightsCount);
    }

    Shader LightsManager::CreateLightObjectShader()
    {
        constexpr std::string_view fragmentSource = R"(
            #version 450 core
            out vec4 FragColor;
            uniform vec3 u_LightBaseColor;
            void main()
            {
                FragColor = vec4(u_LightBaseColor, 1.0);
            }
        )";
        constexpr std::string_view vertexSource = R"(
            #version 450 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Normal;
            layout (location = 2) in vec2 a_UV;
            layout (std140, binding = 0) uniform Matrices
            {
                mat4 u_Projection;
                mat4 u_View;
            };
            uniform mat4 u_Model;
            out VS_VERTEX
            {
                vec3 Position;
                vec3 Normal;
                vec2 UV;
            } Vertex;
            void main()
            {
                vec4 vertexPosition = u_Model * vec4(a_Position, 1.0);
                gl_Position = u_Projection * u_View * vertexPosition;
                Vertex.Position = vertexPosition.xyz;
                Vertex.Normal = normalize(mat3(transpose(inverse(u_Model))) * a_Normal);
                Vertex.UV = a_UV;
            }
        )";
        Shader shader;
        shader.Create({ {ShaderStage::Fragment, fragmentSource}, {ShaderStage::Vertex, vertexSource} });
        return shader;
    }
    
    void LightsManager::DrawLightObjects(Shader& shader, const Model& pointLight, const Model& spotLight)
    {
        shader.Bind();
        Transform transform;
        transform.Scale = glm::vec3(0.2f);
        for (unsigned int i = 0; i < m_PointLightsCount; ++i)
        {
            transform.Position = m_PointLights[i].Position;
            shader.UniformMat4("u_Model", transform.GetModelMatrix());
            shader.UniformF3("u_LightBaseColor", m_PointLights[i].Color.Base);
            pointLight.Draw(shader);
        }
        for (unsigned int i = 0; i < m_SpotLightsCount; ++i)
        {
            transform.Position = m_SpotLights[i].Position;
            shader.UniformMat4("u_Model", transform.GetModelMatrix());
            shader.UniformF3("u_LightBaseColor", m_SpotLights[i].Color.Base);
            spotLight.Draw(shader);
        }
    }

    void LightsManager::CreateGpuBuffer()
    {
        glGenBuffers(1, &m_GpuBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_GpuBuffer);
        glBufferData(GL_UNIFORM_BUFFER, GpuBufferSize(), nullptr, GL_STATIC_DRAW);
    }

    unsigned int LightsManager::GpuBufferSize() const
    {
        unsigned int pointAllocatedSize = sizeof(PointLight) * MaxPointLightCount;
        unsigned int directionalAllocatedSize = sizeof(DirectionalLight) * MaxDirectionalLightCount;
        unsigned int spotAllocatedSize = sizeof(SpotLight) * MaxSpotLightCount;
        return pointAllocatedSize + directionalAllocatedSize + spotAllocatedSize + sizeof(unsigned int) * 3;
    }

    bool LightsManager::DirectionalLightEdit()
    {
        int update = 0;
        if (ImGui::TreeNode("Directional"))
        {
            if (m_DirectionalLightsCount < MaxDirectionalLightCount && ImGui::Button("Add"))
                update += AddDirectionalLight();

            for (unsigned int i = 0; i < m_DirectionalLightsCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(GetLightTreeNodeName(i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    DirectionalLight& light = m_DirectionalLights[i];
                    update += ImGui::DragFloat3("Direction", &light.Direction[0], 0.05f, -1.0f, 1.0f);
                    update += ColorEdit(light.Color);

                    if (ImGui::Button("Remove"))
                        update += RemoveDirectionalLight(i);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        return update > 0;
    }

    bool LightsManager::PointLightEdit()
    {
        int update = 0;
        if (ImGui::TreeNode("Point"))
        {
            if (m_PointLightsCount < MaxPointLightCount && ImGui::Button("Add"))
                update += AddPointLight();

            for (unsigned int i = 0; i < m_PointLightsCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(GetLightTreeNodeName(i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    PointLight& light = m_PointLights[i];
                    update += ImGui::DragFloat3("Position", &light.Position[0], m_ImGuiDragSpeed);
                    update += ColorEdit(light.Color);
                    update += AttenuationEdit(light.Attenuation);

                    if (ImGui::Button("Remove"))
                        update += RemovePointLight(i);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        return update > 0;
    }

    bool LightsManager::SpotLightEdit()
    {
        int update = 0;
        if (ImGui::TreeNode("Spot"))
        {
            if (m_SpotLightsCount < MaxSpotLightCount && ImGui::Button("Add"))
                update += AddSpotLight();

            for (unsigned int i = 0; i < m_SpotLightsCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(GetLightTreeNodeName(i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    SpotLight& light = m_SpotLights[i];
                    update += ImGui::DragFloat3("Position", &light.Position[0], m_ImGuiDragSpeed);
                    update += ImGui::DragFloat3("Direction", &light.Direction[0], 0.05f, -1.0f, 1.0f);
                    update += ColorEdit(light.Color);
                    update += AttenuationEdit(light.Attenuation);

                    float cutOff = glm::degrees(light.CutOff);
                    float outerCutOff = glm::degrees(light.OuterCutOff);
                    update += ImGui::DragFloat("Cut Off", &cutOff, 0.5f, 0.1f, outerCutOff);
                    update += ImGui::DragFloat("Outer Cut Off", &outerCutOff, 0.5f, 0.1f, 100.0f);
                    cutOff = std::clamp(cutOff, 0.1f, outerCutOff);
                    light.OuterCutOff = glm::radians(outerCutOff);
                    light.CutOff = glm::radians(cutOff);

                    if (ImGui::Button("Remove"))
                        update += RemoveSpotLight(i);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        return update > 0;
    }

    bool LightsManager::ColorEdit(LightColor& color)
    {
        int update = 0;
        if (ImGui::TreeNode("Color"))
        {
            update += ImGui::ColorEdit3("Base", &color.Base[0]);
            update += ImGui::ColorEdit3("Specular", &color.Specular[0]);
            update += ImGui::ColorEdit3("Ambient", &color.Ambient[0]);
            update += ImGui::DragFloat("Intensity", &color.Intensity, 0.05f);
            ImGui::TreePop();
        }
        return update > 0;
    }

    bool LightsManager::AttenuationEdit(LightAttenuation& attenuation)
    {
        float distance = attenuation.GetDistance();
        float lastDistance = distance;
        ImGui::DragFloat("Distance", &distance);
        if (lastDistance != distance)
        {
            attenuation.SetDistance(distance);
            return true;
        }
        return false;
    }

    const char* LightsManager::GetLightTreeNodeName(unsigned int index)
    {
        constexpr unsigned int maxBufferSize = 128;
        static char buffer[128];
        snprintf(buffer, maxBufferSize, "Light %u", index + 1);
        return buffer;
    }
}
