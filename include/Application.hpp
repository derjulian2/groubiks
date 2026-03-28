
#pragma once

/**************************************************************
 * @file   Application.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  main-application-interface.
 **************************************************************/

#include <Logging.hpp>
#include <Camera.hpp>
#include <Cube.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <filesystem>

namespace ng
{

    class Window
        : public sf::RenderWindow
    {
    public:

        static
        constexpr u32 depth_buffer_bits = 24;

        static
        constexpr u32 stencil_buffer_bits = 8;

        static
        constexpr u32 antialiasing_level = 0;

        sf::Shader       m_shader;
        sf::Event        m_event;
        sf::Clock        m_clock;

        Window(std::filesystem::path vertShaderPath,
               std::filesystem::path fragShaderPath,
               const u32 width = 1280, 
               const u32 height = 720, 
               std::string_view title = "neogroubiks")
            : sf::RenderWindow(
                sf::VideoMode(width, height), 
                title.data(), 
                sf::Style::Default,
                sf::ContextSettings(
                    depth_buffer_bits,
                    stencil_buffer_bits,
                    antialiasing_level
                )
            ) 
        {
            setActive();
            setVerticalSyncEnabled(true);
            if (!gladLoadGL())
            { throw std::runtime_error("glad failed to load"); }
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            if (!m_shader.loadFromFile(vertShaderPath, fragShaderPath))
            { throw std::runtime_error("failed to load shaders"); }
        }

        f32 aspect_ratio() const { 
            sf::Vector2u sz = getSize(); 
            return static_cast<f32>(sz.x) / static_cast<f32>(sz.y); 
        }

    };

    class Gui : public sf::Drawable
    {
        ng::Window&     m_window;
        ng::RubiksCube& m_cube;
        ng::Camera&     m_camera;

    public:

        Gui(ng::Window& window,
            ng::RubiksCube& cube,
            ng::Camera& cam)
            : m_window(window), m_cube(cube), m_camera(cam) { 
            if (!ImGui::SFML::Init(window))
            { throw std::runtime_error("failed to initialize GUI"); }
        }

        ~Gui()
        { ImGui::SFML::Shutdown(); }

        void update();

        void draw(sf::RenderTarget& target, 
                  sf::RenderStates states = sf::RenderStates::Default) const
        { ImGui::SFML::Render(target); }

        void pollEvents() 
        { ImGui::SFML::ProcessEvent(m_window, m_window.m_event); }
    };

    class Application
    {
    public:
        
        ng::Window       m_window; // init-order is important here!
        ng::Gui          m_gui;
        ng::Camera       m_camera;
        ng::RubiksCube   m_cube;

        Application(std::filesystem::path vertShaderPath,
                    std::filesystem::path fragShaderPath)
            : m_window(vertShaderPath, fragShaderPath) 
            , m_camera(m_window.aspect_ratio())
            , m_gui(m_window, m_cube, m_camera)
        { }

        void execute();
    };

}