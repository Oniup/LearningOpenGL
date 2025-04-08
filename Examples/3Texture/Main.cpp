#include "Common/Shader.h"
#include "Common/Window.h"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <string_view>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb/stb_image.h>

struct Vertex
{
    glm::vec3 Position;
    glm::vec2 UV;
};

unsigned int LoadTexture(const std::string_view& path, bool linearFiltering)
{
    int width, height, channels;
    unsigned char* imageData = stbi_load(path.data(), &width, &height, &channels, 0);
    if (!imageData)
    {
        std::cerr << "Failed to load image at path \"" << path << "\"\n";
        std::exit(-1);
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linearFiltering ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linearFiltering ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);

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
    return texture;
}

int main(int argc, char** argv)
{
    Cm::Window window(PROJECT_NAME, 600, 600);
    Cm::Shader shader({PROJECT_DIR "/TextureShader.frag", PROJECT_DIR "/TextureShader.vert"});

    unsigned int backgroundTexture = LoadTexture((RESOURCE_DIR "/Prototype/texture_07.png"), false);
    unsigned int faceTexture = LoadTexture(RESOURCE_DIR "/awesomeface.png", true);

    Vertex vertices[] = {
        {glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec2(1.0f, 1.0f)},  // top right
        {glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},  // bottom right
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)},  // bottom left
        {glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 1.0f)},  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // First triangle
        1, 2, 3, // Second triangle
    };

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Swap between mixing the two textures or if the smile face alpha isn't 0, then override base texture
    bool useMix = true;

    while (window.IsOpen())
    {
        window.PollEvents();

        glUseProgram(shader.GetGpuId());
        glUniform1i(glGetUniformLocation(shader.GetGpuId(), "u_UseMix"), useMix);

        // Assign the sample 2D to a active texture ID
        glUniform1i(glGetUniformLocation(shader.GetGpuId(), "u_Diffuse"), 0);
        glUniform1i(glGetUniformLocation(shader.GetGpuId(), "u_SmileFace"), 1);

        // Set which texture is what active texture ID
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, faceTexture);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);

        window.SwapBuffers();
        glClearColor(0.2f, 0.5f, 0.7f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glDeleteTextures(1, &backgroundTexture);
    glDeleteTextures(1, &faceTexture);
    return 0;
}
