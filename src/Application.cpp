
#include <Application.hpp>


ng::Window::Window(std::filesystem::path vertShaderPath,
                   std::filesystem::path fragShaderPath,
                   const u32 width, 
                   const u32 height, 
                   std::string_view title)
    : sf::RenderWindow(sf::VideoMode(width, height),
                       title.data(),
                       sf::Style::Default,
                       sf::ContextSettings(depth_buffer_bits, stencil_buffer_bits, antialiasing_level)) 
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


ng::f32 ng::Window::aspect_ratio() const { 
    sf::Vector2u sz = getSize(); 
    return static_cast<f32>(sz.x) / static_cast<f32>(sz.y); 
}



ng::Gui::Gui(ng::Window& window,
             ng::RubiksCube& cube,
             ng::Camera& cam)
    : m_window(window)
    , m_cube(cube)
    , m_camera(cam) 
{ 
    if (!ImGui::SFML::Init(window))
    { throw std::runtime_error("failed to initialize GUI"); }
}


ng::Gui::~Gui()
{ ImGui::SFML::Shutdown(); }


void ng::Gui::pollEvents() 
{ ImGui::SFML::ProcessEvent(m_window, m_window.m_event); }


void ng::Gui::update() 
{
    float angle = 0.f;
    ImGui::SFML::Update(m_window, m_window.m_clock.restart());
    ImGui::Begin("neogroubiks");

    ImGui::InputFloat3("camera position", &m_camera.position().x);
    ImGui::SliderFloat("camera angle", &angle, 0, 360);
    m_camera.front() = { cos(glm::radians(angle)), sin(glm::radians(angle)), 0 };

    ImGui::End();
}


void ng::Gui::draw(sf::RenderTarget& target, 
                   sf::RenderStates states) const
{ ImGui::SFML::Render(target); }



ng::App::App(std::filesystem::path vertShaderPath,
             std::filesystem::path fragShaderPath)
    : m_window(vertShaderPath, fragShaderPath) 
    , m_camera(m_window.aspect_ratio())
    , m_gui(m_window, m_cube, m_camera)
{ }


void ng::App::execute() 
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
