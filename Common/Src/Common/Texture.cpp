#include "Common/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Cm
{
    Texture::Texture()
        : m_GpuId(std::numeric_limits<unsigned int>::max())
    {
    }

    Texture::Texture(const std::string_view& path, TextureFilter filter, bool generateMipmap)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* imageData = stbi_load(path.data(), &width, &height, &channels, 0);
        if (!imageData)
        {
            std::cerr << "Failed to load image at path \"" << path << "\"\n";
            std::exit(-1);
        }

        glGenTextures(1, &m_GpuId);
        glBindTexture(GL_TEXTURE_2D, m_GpuId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (generateMipmap)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter == TextureFilter::Linear ? GL_LINEAR : GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter == TextureFilter::Linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
        }

        int glFormat;
        switch (channels)
        {
        case 1:
            glFormat = GL_RED;
            break;
        case 2:
            glFormat = GL_RG;
            break;
        case 3:
            glFormat = GL_RGB;
            break;
        case 4:
            glFormat = GL_RGBA;
            break;
        default:
            std::cerr << "Invalid number of channels " << channels << "\n";
            std::exit(-1);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::~Texture()
    {
        Destroy();
    }

    Texture::Texture(Texture&& texture)
        : m_GpuId(texture.m_GpuId)
    {
        texture.m_GpuId = std::numeric_limits<unsigned int>::max();
    }

    Texture& Texture::operator=(Texture&& texture)
    {
        Destroy();
        m_GpuId = texture.m_GpuId;
        texture.m_GpuId = std::numeric_limits<unsigned int>::max();
        return *this;
    }

    void Texture::Bind(unsigned int index)
    {
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, m_GpuId);
    }

    void Texture::Destroy()
    {
        if (m_GpuId != std::numeric_limits<unsigned int>::max())
        {
            glDeleteTextures(1, &m_GpuId);
            m_GpuId = std::numeric_limits<unsigned int>::max();
        }
    }
}
