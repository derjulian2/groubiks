
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

      void _M_update();

    public:

      Camera(f32 aspect_ratio,
            const glm::vec3& pos = { 0.0, 0.0, 0.0 },
            const glm::vec3& front = { 1.0, 0.0, 0.0 },
            f32 fov_angle    = glm::radians(45.0));

            glm::vec3& position();    
      const glm::vec3& position() const;

            glm::vec3& front();     
      const glm::vec3& front() const;

            f32 aspect_ratio();    
      const f32 aspect_ratio() const;

            f32 fov_angle();
      const f32 fov_angle() const;

      void apply(sf::Shader& shader,
                  std::string_view projMatUniformName = "proj",
                  std::string_view viewMatUniformName = "view");

    };

}