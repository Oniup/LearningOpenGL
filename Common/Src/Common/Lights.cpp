#include "Common/Lights.h"
#include <imgui/imgui.h>

namespace Cm
{
    LightAttenuation::LightAttenuation()
    {
        SetDistance(50.0f);
    }

    void LightAttenuation::SetDistance(float distance)
    {
        constexpr float a = 0.640f;
        constexpr float b = 0.130f;
        constexpr float c = 83.300f;
        Linear = a / (distance * b);
        Quadratic = c / (distance * distance);
    }

    float LightAttenuation::GetDistance() const
    {
        constexpr float a = 0.640f;
        constexpr float b = 0.130f;
        constexpr float c = 83.300f;

        float fromLinear = a / (Linear * b);
        return fromLinear;
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

    void LightsManager::ImGuiEdit()
    {
        int valChanged = 0;
        if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("Drag Value Speed", &m_ImGuiDragSpeed, 0.1f);
            valChanged += DirectionalLightEdit();
            ImGui::Separator();
            valChanged += PointLightEdit();
            ImGui::Separator();
            valChanged += SpotLightEdit();
        }
        if (valChanged > 0)
            UpdateGpuBuffer();
    }

    void LightsManager::Bind(unsigned int uboBindingIndex)
    {
        glBindBufferRange(GL_UNIFORM_BUFFER, uboBindingIndex, m_GpuBuffer, 0, GpuBufferSize());
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
        glBufferSubData(GL_UNIFORM_BUFFER, countOffset + 0 * sizeof(unsigned int), sizeof(unsigned int), &m_PointLightsCount);
        glBufferSubData(GL_UNIFORM_BUFFER, countOffset + 1 * sizeof(unsigned int), sizeof(unsigned int), &m_DirectionalLightsCount);
        glBufferSubData(GL_UNIFORM_BUFFER, countOffset + 2 * sizeof(unsigned int), sizeof(unsigned int), &m_SpotLightsCount);
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
        int valChanged = 0;
        if (ImGui::TreeNode("Directional"))
        {
            if (m_DirectionalLightsCount < MaxDirectionalLightCount && ImGui::Button("Add"))
            {
                ++m_DirectionalLightsCount;
                valChanged += 1;
            }
            for (unsigned int i = 0; i < m_DirectionalLightsCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(GetLightTreeNodeName(i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    DirectionalLight& light = m_DirectionalLights[i];
                    valChanged += ImGui::DragFloat3("Direction", &light.Direction[0], 0.05f, -1.0f, 1.0f);
                    valChanged += ColorEdit(light.Color);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        return valChanged > 0;
    }

    bool LightsManager::PointLightEdit()
    {
        int valChanged = 0;
        if (ImGui::TreeNode("Point"))
        {
            if (m_PointLightsCount < MaxPointLightCount && ImGui::Button("Add"))
            {
                ++m_PointLightsCount;
                valChanged += 1;
            }
            for (unsigned int i = 0; i < m_PointLightsCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(GetLightTreeNodeName(i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    PointLight& light = m_PointLights[i];
                    valChanged += ImGui::DragFloat3("Position", &light.Position[0], m_ImGuiDragSpeed);
                    valChanged += ColorEdit(light.Color);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        return valChanged > 0;
    }

    bool LightsManager::SpotLightEdit()
    {
        int valChanged = 0;
        if (ImGui::TreeNode("Spot"))
        {
            if (m_SpotLightsCount < MaxSpotLightCount && ImGui::Button("Add"))
            {
                ++m_SpotLightsCount;
                valChanged += 1;
            }
            for (unsigned int i = 0; i < m_SpotLightsCount; ++i)
            {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(GetLightTreeNodeName(i), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    SpotLight& light = m_SpotLights[i];
                    valChanged += ImGui::DragFloat3("Position", &light.Position[0], m_ImGuiDragSpeed);
                    valChanged += ImGui::DragFloat3("Direction", &light.Direction[0], 0.05f, -1.0f, 1.0f);
                    valChanged += ColorEdit(light.Color);
                    valChanged += AttenuationEdit(light.Attenuation);

                    float cutOff = glm::degrees(light.CutOff);
                    float outerCutOff = glm::degrees(light.OuterCutOff);
                    valChanged += ImGui::DragFloat("Cut Off", &cutOff, 0.5f, 0.1f, outerCutOff);
                    valChanged += ImGui::DragFloat("Outer Cut Off", &outerCutOff, 0.5f, 0.1f, 100.0f);
                    light.OuterCutOff = glm::radians(outerCutOff);
                    light.CutOff = glm::radians(cutOff);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
        return valChanged > 0;
    }

    bool LightsManager::ColorEdit(LightColor& color)
    {
        int valChanged = 0;
        if (ImGui::TreeNode("Color"))
        {
            valChanged += ImGui::ColorEdit3("Base", &color.Base[0]);
            valChanged += ImGui::ColorEdit3("Specular", &color.Specular[0]);
            valChanged += ImGui::ColorEdit3("Ambient", &color.Ambient[0]);
            valChanged += ImGui::DragFloat("Intensity", &color.Intensity, 0.05f);
            ImGui::TreePop();
        }
        return valChanged > 0;
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
