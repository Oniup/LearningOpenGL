#pragma once

#include <string_view>
#include <glm/glm.hpp>

namespace Cm
{
    glm::ivec4 RgbaToColor(const glm::ivec4& color);
    glm::vec4 RgbaToColor(int r, int g, int b, int a = 255);
    glm::ivec4 HexToColor32(const std::string_view& hexColor);
    glm::ivec4 HexToColor32(uint32_t hexValue);
    glm::vec4 HexToColor(const std::string_view& hexColor);
    glm::vec4 HexToColor(uint32_t hexValue);
}
