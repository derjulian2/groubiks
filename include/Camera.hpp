
#pragma once

/**************************************************************
 * @file   Camera.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  3D-Camera class that manages projection
 *         and view-matrices.
 **************************************************************/

#include <Gl.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <string_view>

namespace ng
{
    /**************************************************************
     * @brief handles all vertex-transformation logic
     *        to create the 3D-rendering-effect.
     *
     *        requires appropriate vertex-shader-layout.
     **************************************************************/
    class Camera
    {
    private:
    
        glm::vec3 m_pos;
        glm::vec3 m_front;
        f32       m_aspect_ratio;
        f32       m_fov_angle;

        glm::mat4 m_proj;
        glm::mat4 m_view;

        void __update();

    public:

        Camera(const glm::vec3& pos = { 0.0, 0.0, 0.0 },
               const glm::vec3& front = { 1.0, 0.0, 0.0 },
               f32 fov_angle    = glm::radians(45.0),
               f32 aspect_ratio = 640.0/480.0) 
        : m_pos(pos), 
          m_front(front), 
          m_fov_angle(fov_angle), 
          m_aspect_ratio(aspect_ratio) 
        { }

              glm::vec3& position()       { return m_pos; }
        const glm::vec3& position() const { return m_pos; }

              glm::vec3& front()       { return m_front; }
        const glm::vec3& front() const { return m_front; }

              f32 aspect_ratio()       { return m_aspect_ratio; }
        const f32 aspect_ratio() const { return m_aspect_ratio; }

              f32 fov_angle()       { return m_fov_angle; }
        const f32 fov_angle() const { return m_fov_angle; }

        void apply(sf::Shader& shader,
            std::string_view projMatUniformName = "proj",
            std::string_view viewMatUniformName = "view"
        );

    };

    inline sf::Glsl::Mat4 convert(const glm::mat4& m) {
        return sf::Glsl::Mat4(&m[0][0]);
    }
}