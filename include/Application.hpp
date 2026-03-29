
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
               std::string_view title = "neogroubiks");

        f32 aspect_ratio() const;

    };

    class Gui : public sf::Drawable
    {
        ng::Window&     m_window;
        ng::RubiksCube& m_cube;
        ng::Camera&     m_camera;

    public:

        Gui(ng::Window& window,
            ng::RubiksCube& cube,
            ng::Camera& cam);

        ~Gui();

        void pollEvents();

        void update();

        void draw(sf::RenderTarget& target, 
                  sf::RenderStates states = sf::RenderStates::Default) const;
    };

    class App
    {
    public:
        
        ng::Window       m_window; // init-order is important here!
        ng::Gui          m_gui;
        ng::Camera       m_camera;
        ng::RubiksCube   m_cube;

        App(std::filesystem::path vertShaderPath,
                    std::filesystem::path fragShaderPath);

        void execute();
    };

}