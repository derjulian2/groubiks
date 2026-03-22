
#include <Camera.hpp>


void ng::Camera::__update() 
{
    /* perspective-projection */
    glm::vec2 frustum = { 0.1f, 100.0f };
    m_proj = glm::perspective(fov_angle(), 
        aspect_ratio(),
        frustum.x,
        frustum.y
    );

    /* view-space */
    glm::vec3 up    = { 0.0, 0.0, 1.0 };
    glm::vec3 right = glm::normalize(glm::cross(front(), up));

    m_view = glm::lookAt(position(), 
        position() + glm::normalize(front()), 
        up
    );
}


void ng::Camera::apply(sf::Shader& shader,
            std::string_view projMatUniformName,
            std::string_view viewMatUniformName)
{
    __update();
    shader.setUniform(projMatUniformName.data(), ng::convert(m_proj));
    shader.setUniform(viewMatUniformName.data(), ng::convert(m_view));
}