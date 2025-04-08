#pragma once

namespace Cm
{
    enum TextureFilter
    {
        Nearest,
        Linear,
    };

    class Texture
    {
    public:
        Texture(const std::string_view& path, TextureFilter filter, bool generateMipmap = true);
        ~Texture();

        unsigned int GetGpuId() const
        {
            return m_GpuId;
        }

    private:
        unsigned int m_GpuId;
    };
}
