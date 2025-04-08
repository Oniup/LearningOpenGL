#include "Common/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

int main(int argc, char** argv)
{
    Cm::Window window(PROJECT_NAME, 600, 600, Cm::Window::DefaultFlags);

    while (window.IsOpen())
    {
        glfwPollEvents();

        window.SwapBuffers();
        glClearColor(0.2f, 0.5f, 0.7f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    return 0;
}
