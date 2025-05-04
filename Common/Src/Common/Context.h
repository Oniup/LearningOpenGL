#pragma once

#include <glm/glm.hpp>

#include "Common/Window.h"

namespace Cm
{
    class Context
    {
    public:
        static constexpr glm::vec4 DefaultClearColor = glm::vec4(0.2f, 0.5f, 0.7f, 1.0f);

    public:
        Context(const std::string_view& title, WindowFlags flags = Window::DefaultFlags);
        Context(const std::string_view& title, int width, int height, WindowFlags flags = Window::DefaultFlags);
        ~Context();

    public:
        Window& GetWindow()
        {
            return m_Window;
        }

        const Window& GetWindow() const
        {
            return m_Window;
        }

        glm::vec4& GetClearColor()
        {
            return  m_ClearColor;
        }

        const glm::vec4& GetClearColor() const
        {
            return  m_ClearColor;
        }

        float GetDeltaTime()
        {
            return m_DeltaTime;
        }

        bool IsImGuiEnabled() const
        {
            return m_ImGuiEnabled;
        }

        void EnableImGui();
        void SetClearOptions(int clearOptions);

        bool BeginFrame();
        void EndFrame();

    private:
        void SetTheme();

    private:
        Window m_Window;
        bool m_ImGuiEnabled;
        float m_DeltaTime;
        glm::vec4 m_ClearColor;
        int m_ClearOptions;
    };
}
