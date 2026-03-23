
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
    /*
     * helper-class to manage correct initialization-order:
     * 1. create sf::RenderWindow
     * 2. initialize glad and shaders
     * 3. initialize imgui-SFML-backend
     */
    class Window
    {
    public:

        sf::RenderWindow m_renderwindow;
        sf::Shader       m_shader;
        sf::Event        m_event;
        sf::Clock        m_clock;

        Window(std::filesystem::path vertShaderPath,
               std::filesystem::path fragShaderPath,
               const u32 width = 640, 
               const u32 height = 480, 
               std::string_view title = "neogroubiks");

        ~Window();
        
    };

    class Application
    {
    public:
        /* mind the initialization-order! window first, bc cube uses GlResources! */
        ng::Window       m_window;
        ng::Camera       m_camera;
        ng::Cube         m_cube;

        Application(std::filesystem::path vertShaderPath,
                    std::filesystem::path fragShaderPath)
            : m_window(vertShaderPath, fragShaderPath)
        { }

        ~Application();

        void execute();
    };

}