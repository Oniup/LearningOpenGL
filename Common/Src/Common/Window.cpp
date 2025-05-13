#include "Common/Window.h"

namespace Cm
{
    void ErrorCallback(int errorCode, const char* description)
    {
        std::cerr << "GLFW ERROR (" << errorCode << "): " << description << "\n";
    }

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    Window::Window(const std::string_view& title, WindowFlags flags)
        : Window(title, -1, -1, flags)
    {
    }

    Window::Window(const std::string_view& title, int width, int height, WindowFlags flags)
        : m_Window(nullptr), m_Flags(flags)
    {
        glfwInit();
        glfwSetErrorCallback(ErrorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if ONU_PLATFORM_MACOS
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

        glfwWindowHint(GLFW_RESIZABLE, flags & WindowFlags_Resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, flags & WindowFlags_TransparentFrameBuffer ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, flags & WindowFlags_Borderless ? GLFW_FALSE : GLFW_TRUE);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);

        bool setSize = width == -1 || height == -1;
        if (flags & WindowFlags_ModeFullscreen || (setSize && flags & WindowFlags_Borderless))
        {
            width = vidMode->width;
            height = vidMode->height;
        }
        else if (setSize)
        {
            width = vidMode->width - vidMode->width / 3;
            height = vidMode->height - vidMode->height / 3;
        }

        m_Window = glfwCreateWindow(width, height, title.data(), flags & WindowFlags_ModeFullscreen ? monitor : nullptr, nullptr);
        assert(m_Window);
        glfwMakeContextCurrent(m_Window);

        int centerX = (vidMode->width - width) / 2;
        int centerY = (vidMode->height - height) / 2;
        glfwSetWindowPos(m_Window, centerX, centerY);

        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

        // Set callbacks
        glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
    }

    Window::~Window()
    {
        if (m_Window)
            glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    bool Window::IsOpen() const
    {
        return !glfwWindowShouldClose(m_Window);
    }

    void Window::ShouldClose(bool shouldClose)
    {
        glfwSetWindowShouldClose(m_Window, shouldClose);
    }

    int Window::GetWidth() const
    {
        int width;
        glfwGetFramebufferSize(m_Window, &width, nullptr);
        return width;
    }

    int Window::GetHeight() const
    {
        int height;
        glfwGetFramebufferSize(m_Window, nullptr, &height);
        return height;
    }

    void Window::GetSize(int* width, int* height) const
    {
        glfwGetFramebufferSize(m_Window, width, height);
    }

    int Window::GetXPosition() const
    {
        int x;
        glfwGetWindowPos(m_Window, &x, nullptr);
        return x;
    }

    int Window::GetYPosition() const
    {
        int y;
        glfwGetWindowPos(m_Window, nullptr, &y);
        return y;
    }

    void Window::GetPosition(int* x, int* y) const
    {
        glfwGetWindowPos(m_Window, x, y);
    }

    void Window::Maximize()
    {
        glfwMaximizeWindow(m_Window);
    }

    void Window::Restore()
    {
        glfwRestoreWindow(m_Window);
    }

    std::string_view Window::Title() const
    {
        return glfwGetWindowTitle(m_Window);
    }

    GLFWmonitor* Window::GetInternalCurrentMonitor() const
    {
        int xPos, yPos;
        GetPosition(&xPos, &yPos);

        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        for (int i = 0; i < count; ++i)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            int mX, mY;
            glfwGetMonitorPos(monitors[i], &mX, &mY);
            if (xPos >= mX && xPos < (mX + mode->width))
            {
                if (yPos >= mY && yPos < (mY + mode->height))
                {
                    return monitors[i];
                }
            }
        }
        return nullptr;
    }
}
