
#include <Application.hpp>

void ng::Gui::update() {
    float angle = 0.f;
    ImGui::SFML::Update(m_window, m_window.m_clock.restart());
    ImGui::Begin("neogroubiks");

    ImGui::InputFloat3("camera position", &m_camera.position().x);
    ImGui::SliderFloat("camera angle", &angle, 0, 360);
    m_camera.front() = { cos(glm::radians(angle)), sin(glm::radians(angle)), 0 };

    ImGui::End();
}

void ng::Application::execute() 
{
    sf::Texture tex;
    tex.loadFromFile(NEOGROUBIKS_WALL_TEX_PATH);
    m_cube.update();
    while (m_window.isOpen()) {
        while (m_window.pollEvent(m_window.m_event)) {
            m_gui.pollEvents();
            if (m_window.m_event.type == sf::Event::Closed) {
                m_window.close();
            }
        }
        
        m_gui.update();
        
        m_window.clear(sf::Color::White);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* bind shader for 3D-drawing */
        sf::Shader::bind(&m_window.m_shader);
        m_camera.apply(m_window.m_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.getNativeHandle());
        m_window.draw(m_cube);
        sf::Shader::bind(nullptr);
        /* be careful with other GUI-rendering: */
        /* GL-states need to be preserved */
        m_window.pushGLStates();
        m_window.draw(m_gui);
        m_window.popGLStates();

        m_window.display();
    }
}
