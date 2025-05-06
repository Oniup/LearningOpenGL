#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Common/Context.h"
#include "Common/Shader.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Color;
};

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME, 600, 600);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT);
    Cm::Shader shader({PROJECT_DIR "/VertexColored.frag", PROJECT_DIR "/VertexColored.vert"});

    Vertex vertices[] = {
        {glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},  // top right
        {glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},  // bottom right
        {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},  // bottom left
        {glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3(1.0f)},              // top left
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    while (context.BeginFrame())
    {
        glUseProgram(shader.GetGpuId());
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);

        context.EndFrame();
    }
    return 0;
}
