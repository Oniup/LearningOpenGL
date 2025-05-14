#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>
#include <glm/glm.hpp>

namespace Cm
{
    enum class ShaderStage
    {
        Vertex,
        Fragment,
        Geometry,
        Compute,
        Invalid,
    };

    constexpr std::array<std::string_view, 4> ShaderFileExtensions = {
        ".vert",
        ".frag",
        ".geom",
        ".comp",
    };

    class Shader
    {
    public:
        Shader();
        Shader(const std::vector<std::string_view>& paths);
        ~Shader();

        Shader(Shader&& shader);
        Shader& operator=(Shader&& shader);

        unsigned int GetGpuId() const
        {
            return m_GpuId;
        }

        void Bind();
        void Destroy();
        void Create(const std::vector<std::pair<ShaderStage, std::string>>& sources);
        void Create(const std::vector<std::pair<ShaderStage, std::string_view>>& sources);

        void UniformI(const std::string_view& location, int val);
        void UniformI2(const std::string_view& location, const glm::ivec2& vec);
        void UniformI3(const std::string_view& location, const glm::ivec3& vec);
        void UniformI4(const std::string_view& location, const glm::ivec4& vec);

        void UniformU(const std::string_view& location, unsigned int val);
        void UniformU2(const std::string_view& location, const glm::uvec2& vec);
        void UniformU3(const std::string_view& location, const glm::uvec3& vec);
        void UniformU4(const std::string_view& location, const glm::uvec4& vec);

        void UniformF(const std::string_view& location, float val);
        void UniformF2(const std::string_view& location, const glm::vec2& vec);
        void UniformF3(const std::string_view& location, const glm::vec3& vec);
        void UniformF4(const std::string_view& location, const glm::vec4& vec);

        void UniformMat2(const std::string_view& location, glm::mat2 mat);
        void UniformMat3(const std::string_view& location, glm::mat3 mat);
        void UniformMat4(const std::string_view& location, glm::mat4 mat);

    private:
        std::pair<ShaderStage, std::string> ReadSource(const std::string_view& path);
        int ShaderStageToOpenGL(ShaderStage stage) const;
        std::string_view ShaderStageToString(ShaderStage stage);
        unsigned int CreateShaderInstance(ShaderStage stage, const std::string_view& source);
        void CreateProgram(const std::vector<unsigned int>& shaders);

    private:
        unsigned int m_GpuId;
    };
}
