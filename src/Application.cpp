
#include <Application.hpp>


ng::Application::~Application() {
    shutdown();
}


void ng::Application::initialize(std::filesystem::path vertShaderPath,
    std::filesystem::path fragShaderPath)
{
    if (!gladLoadGL()) 
    { throw std::runtime_error("failed to load glad"); }
    if (!m_shader.loadFromFile(vertShaderPath, fragShaderPath))
    { throw std::runtime_error("failed to load shaders"); }
    if (!ImGui::SFML::Init(m_window))
    { throw std::runtime_error("failed to load ImGui"); }
}


void ng::Application::execute() 
{
    ng::Cube cube;
    cube.update();
    m_window.setActive();
    float Cpos[3] = { 0, 0, 0};
    float angle = 0.0;
    while (m_window.isOpen()) {
        while (m_window.pollEvent(m_event)) {
            ImGui::SFML::ProcessEvent(m_window, m_event);
            if (m_event.type == sf::Event::Closed) {
                m_window.close();
            }
        }

        ImGui::SFML::Update(m_window, m_clock.restart());

        ImGui::Begin("neogroubiks");
        ImGui::InputFloat3("Camera Position", &Cpos[0]);
        ImGui::SliderFloat("Camera angle", &angle, 0.0, 360.0);
        ImGui::End();
        
        m_camera.position() = glm::vec3(Cpos[0], Cpos[1], Cpos[2]);
        m_camera.front() = glm::vec3(cos(glm::radians(angle)), sin(glm::radians(angle)), 0.0);

        m_window.clear(sf::Color::White);
        sf::Shader::bind(&m_shader);
        m_camera.apply(m_shader);
        /* 3D-rendering of the cube */
        m_window.draw(cube);
        sf::Shader::bind(nullptr);
        /* render gui on top */
        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}


void ng::Application::shutdown() {
    ImGui::SFML::Shutdown();
}