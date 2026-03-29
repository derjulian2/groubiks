
#include <Camera.hpp>

ng::Camera::Camera(f32 aspect_ratio,
                   const glm::vec3& pos,
                   const glm::vec3& front,
                   f32 fov_angle) 
    : m_aspect_ratio(aspect_ratio)
    , m_pos(pos)
    , m_front(front)
    , m_fov_angle(fov_angle)
{ }


void ng::Camera::_M_update() 
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


      glm::vec3& ng::Camera::position()       { return m_pos; }
const glm::vec3& ng::Camera::position() const { return m_pos; }


      glm::vec3& ng::Camera::front()       { return m_front; }
const glm::vec3& ng::Camera::front() const { return m_front; }


      ng::f32 ng::Camera::aspect_ratio()       { return m_aspect_ratio; }
const ng::f32 ng::Camera::aspect_ratio() const { return m_aspect_ratio; }


      ng::f32 ng::Camera::fov_angle()       { return m_fov_angle; }
const ng::f32 ng::Camera::fov_angle() const { return m_fov_angle; }


void ng::Camera::apply(sf::Shader& shader,
                       std::string_view projMatUniformName,
                       std::string_view viewMatUniformName)
{
    _M_update();
    shader.setUniform(projMatUniformName.data(), ng::matrix_cast<sf::Glsl::Mat4>(m_proj));
    shader.setUniform(viewMatUniformName.data(), ng::matrix_cast<sf::Glsl::Mat4>(m_view));
}