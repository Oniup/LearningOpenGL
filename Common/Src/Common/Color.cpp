#include "Common/Color.h"

namespace Cm
{
    size_t HexCharToInt(char ch)
    {
        if (ch >= '0' && ch <= '9')
            return ch - '0';
        if (ch >= 'A' && ch <= 'F')
            return ch - 'A' + 10;
        if (ch >= 'a' && ch <= 'f')
            return ch - 'a' + 10;
        return 0;
    }

    glm::ivec4 RgbaToColor(const glm::ivec4& color)
    {
        return glm::vec4(color.r / 255, color.g / 255, color.b / 255, color.a / 255);
    }

    glm::vec4 RgbaToColor(int r, int g, int b, int a)
    {
        return glm::vec4(r / 255, g / 255, b / 255, a / 255);
    }

    glm::ivec4 HexToColor32(const std::string_view& hexColor)
    {
        if (hexColor.empty())
        {
            std::cerr << "Hex color is empty\n";
            return glm::vec4(0.0f);
        }

        std::string_view strColor;
        if (hexColor[0] == '#')
            strColor = hexColor.substr(1);
        else
            strColor = hexColor;

        size_t count = strColor.size() / 2;
        if (!(count >= 3 && count <= 4))
        {
            std::cerr << "Invalid hex color value: " << strColor << "\n";
            return glm::vec4(0.0f);
        }

        glm::ivec4 color(1);
        for (size_t i = 0; i < count; ++i)
        {
            size_t offset = i * 2;
            color[i] = (HexCharToInt(strColor[offset]) << 4) + HexCharToInt(strColor[offset + 1]);
        }
        return color;
    }

    glm::ivec4 HexToColor32(uint32_t hexValue)
    {
        glm::ivec4 color;
        color.r = (hexValue >> 24) & 0xFF;
        color.g = (hexValue >> 16) & 0xFF;
        color.b = (hexValue >> 8) & 0xFF;
        color.a = hexValue & 0xFF;
        return color;
    }

    glm::vec4 HexToColor(const std::string_view& hexColor)
    {
        glm::ivec4 rgba = HexToColor32(hexColor);
        return RgbaToColor(rgba);
    }

    glm::vec4 HexToColor(uint32_t hexValue)
    {
        glm::ivec4 rgba = HexToColor32(hexValue);
        return RgbaToColor(rgba);
    }
}
