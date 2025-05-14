#pragma once

#include <glm/glm.hpp>

#include "Common/Shader.h"
#include "Common/Mesh.h"

namespace Cm
{
    struct LightColor
    {
        alignas(16) glm::vec3 Base;
        alignas(16) glm::vec3 Ambient;
        alignas(16) glm::vec3 Specular;
        float Intensity;

	public:
		void SetToDefault();
    };

    struct LightAttenuation
    {
        float Linear;
        float Quadratic;

    public:
		void SetToDefault();

        float GetDistance() const;
        void SetDistance(float distance);
    };

    struct alignas(16) PointLight
    {
        alignas(16) glm::vec3 Position;
        LightColor Color;
        LightAttenuation Attenuation;
    };

    struct alignas(16) DirectionalLight
    {
        alignas(16) glm::vec3 Direction;
        LightColor Color;
    };

    struct alignas(16) SpotLight
    {
        alignas(16) glm::vec3 Position;
        alignas(16) glm::vec3 Direction;
        LightColor Color;
        LightAttenuation Attenuation;
        float CutOff;
		float OuterCutOff;
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

        bool ImGuiEdit();
        void Bind(unsigned int uboBindingIndex);
        void UpdateGpuBuffer();

		static Shader CreateLightObjectShader();
		void DrawLightObjects(Shader& shader, const Model& pointLight, const Model& spotLight);

		bool AddPointLight();
		bool AddDirectionalLight();
		bool AddSpotLight();

		bool RemovePointLight(unsigned int index);
		bool RemoveDirectionalLight(unsigned int index);
		bool RemoveSpotLight(unsigned int index);

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
