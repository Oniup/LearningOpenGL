#pragma once

#include <array>
#include <string>
#include <utility>
#include <vector>

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
        Shader(const std::vector<std::string_view>& paths);
        ~Shader();

        unsigned int GetGpuId() const
        {
            return m_GpuId;
        }

    private:
        std::pair<ShaderStage, std::string> ReadSource(const std::string_view& path);
        int ShaderStageToOpenGL(ShaderStage stage) const;

    private:
        unsigned int m_GpuId = -1;
    };
}
