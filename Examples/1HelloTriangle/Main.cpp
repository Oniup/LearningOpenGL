#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Common/Window.h"

int LoadShader(const std::string_view& fragment, const std::string_view& vertex)
{
    unsigned int vertexShader   = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    constexpr int infoLogLength = 512;

    for (int i = 0; i < 2; i++)
    {
        unsigned int shader;
        std::string_view shaderPath;
        if (i == 0)
        {
            shader = vertexShader;
            shaderPath = vertex;
        }
        else
        {
            shader = fragmentShader;
            shaderPath = fragment;
        }

        std::FILE* file = std::fopen(shaderPath.data(), "rb");
        assert(file);
        std::fseek(file, 0, SEEK_END);
        int length = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);

        std::string source;
        source.resize(length);
        std::fread(source.data(), 1, source.size(), file);
        std::fclose(file);

        // Shader Source: https://docs.gl/gl4/glShaderSource
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[infoLogLength];
            glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog);
            std::cerr << "Failed to compile " << (i == 0 ? "vertex" : "fragment") << " shader: " << infoLog << "\n";
        }
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[infoLogLength];
        glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog);
        std::cerr << "Failed to link shader program's vertex and fragment shaders: " << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

int main(int argc, char** argv)
{
    Cm::Window window(PROJECT_NAME, 600, 600);

    unsigned int shader = LoadShader(PROJECT_DIR "/Fragment.glsl", PROJECT_DIR "/Vertex.glsl");

    unsigned int vao;
    unsigned int vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glm::vec3 vertices[] = {
        glm::vec3(-0.5f, -0.5f, 0.0f), // Left
        glm::vec3(0.5f, -0.5f, 0.0f),  // Right
        glm::vec3(0.0f, 0.5f, 0.0f),   // Top
    };

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    while (window.IsOpen())
    {
        window.PollEvents();

        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.SwapBuffers();
        glClearColor(0.2f, 0.5f, 0.7f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    return 0;
}
