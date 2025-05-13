#pragma once

#include <glm/glm.hpp>

namespace Cm
{
    struct LightColor
    {
        alignas(16) glm::vec3 Base = glm::vec3(1.0f);
        alignas(16) glm::vec3 Ambient = glm::vec3(0.1f);
        alignas(16) glm::vec3 Specular = glm::vec3(1.0f);
        float Intensity = 1.0f;
    };

    struct LightAttenuation
    {
        float Linear;
        float Quadratic;

    public:
        LightAttenuation();

        void SetDistance(float distance);
        float GetDistance() const;
    };

    struct alignas(16) PointLight
    {
        alignas(16) glm::vec3 Position = glm::vec3(0.0f);
        LightColor Color;
        LightAttenuation Attenuation;
    };

    struct alignas(16) DirectionalLight
    {
        alignas(16) glm::vec3 Direction = glm::vec3(0.2f, -1.0f, 0.6f);
        LightColor Color;
    };

    struct alignas(16) SpotLight
    {
        alignas(16) glm::vec3 Position = glm::vec3(0.0f);
        alignas(16) glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
        LightColor Color;
        LightAttenuation Attenuation;
        float CutOff = glm::radians(12.0f);
        float OuterCutOff = glm::radians(17.0f);
    };

    class LightsManager
    {
    public:
        static constexpr unsigned int MaxPointLightCount = 20;
        static constexpr unsigned int MaxDirectionalLightCount = 5;
        static constexpr unsigned int MaxSpotLightCount = 20;

    public:
        LightsManager();
        ~LightsManager();

        void ImGuiEdit();
        void Bind(unsigned int uboBindingIndex);
        void UpdateGpuBuffer();

        const PointLight* GetPointLights() const
        {
            return m_PointLights;
        }

        const DirectionalLight* GetDirectionalLights() const
        {
            return m_DirectionalLights;
        }

        const SpotLight* GetSpotLights() const
        {
            return m_SpotLights;
        }

        unsigned int GetPointLightCount() const
        {
            return m_PointLightsCount;
        }

        unsigned int GetDirectionalLightCount() const
        {
            return m_DirectionalLightsCount;
        }

        unsigned int GetSpotLightsCount() const
        {
            return m_SpotLightsCount;
        }

    private:
        void CreateGpuBuffer();
        unsigned int GpuBufferSize() const;

        bool DirectionalLightEdit();
        bool PointLightEdit();
        bool SpotLightEdit();

        bool ColorEdit(LightColor& color);
        bool AttenuationEdit(LightAttenuation& attenuation);
        const char* GetLightTreeNodeName(unsigned int index);

    private:
        unsigned int m_GpuBuffer;
        unsigned int m_PointLightsCount;
        unsigned int m_DirectionalLightsCount;
        unsigned int m_SpotLightsCount;
        PointLight m_PointLights[MaxPointLightCount];
        DirectionalLight m_DirectionalLights[MaxDirectionalLightCount];
        SpotLight m_SpotLights[MaxSpotLightCount];

        float m_ImGuiDragSpeed = 0.2f;
    };
}
