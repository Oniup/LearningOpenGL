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
        Texture();
        Texture(const std::string_view& path, TextureFilter filter, bool generateMipmap = true);
        ~Texture();

        Texture(Texture&& texture);
        Texture& operator=(Texture&& texture);

        unsigned int GetGpuId() const
        {
            return m_GpuId;
        }

        void Destroy();

    private:
        unsigned int m_GpuId;
    };
}
