#pragma once

#include <string_view>

// Forward declare
struct GLFWwindow;
struct GLFWmonitor;

namespace Cm
{
    using WindowFlags = int;
    enum WindowFlags_
    {
        WindowFlags_None = 0,
        WindowFlags_Resizable = 1 << 0,
        WindowFlags_TransparentFrameBuffer = 1 << 1,
        WindowFlags_Borderless = 1 << 2,
        WindowFlags_ModeFullscreen = 1 << 3,
        WindowFlags_ModeWindowed = 1 << 4,
    };

    typedef void(*PfnWindowDpiChange)(float xScale, float yScale);

    class Window
    {
    public:
        static constexpr WindowFlags DefaultFlags = WindowFlags_Resizable;

    public:
        Window(const std::string_view& title, WindowFlags flags = DefaultFlags);
        Window(const std::string_view& title, int width, int height, WindowFlags flags = DefaultFlags);
        ~Window();

    public:
        GLFWwindow* GetInternalWindow()
        {
            return m_Window;
        }

        GLFWwindow* GetInternalWindow() const
        {
            return m_Window;
        }

        WindowFlags GetFlags() const
        {
            return m_Flags;
        }

    public:
        void PollEvents();
        void SwapBuffers();
        bool IsOpen() const;
        void ShouldClose(bool shouldClose);

        int GetWidth() const;
        int GetHeight() const;
        void GetSize(int* width, int* height) const;

        int GetXPosition() const;
        int GetYPosition() const;
        void GetPosition(int* x, int* y) const;

        void Maximize();
        void Restore();

        std::string_view Title() const;

        GLFWmonitor* GetInternalCurrentMonitor() const;

    private:
        GLFWwindow* m_Window;
        WindowFlags m_Flags;
    };
}
