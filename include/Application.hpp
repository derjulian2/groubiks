
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

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <filesystem>

namespace ng
{

    class Application
    {
    public:
        sf::RenderWindow m_window;
        sf::Event        m_event;

        sf::Shader       m_shader;
        sf::Clock        m_clock;

        ng::Camera       m_camera;

        Application(const u32 width = 640, 
            const u32 height = 480, 
            std::string_view title = "neogroubiks")
        : m_window(sf::VideoMode(width, height), title.data())
        { }

        ~Application();

        void initialize(std::filesystem::path vertShaderPath,
            std::filesystem::path fragShaderPath
        );

        void execute();
    
        void shutdown();
    };

}