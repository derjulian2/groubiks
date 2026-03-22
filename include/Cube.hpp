
#pragma once

/**************************************************************
 * @file   Cube.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  3D-cube rendering-primitive.
 **************************************************************/

#include <Vertex.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <array>

namespace ng
{
    /**************************************************************
     * @brief cube-rendering-primitive defined by 
     *        a center-point, a front vector and a sidelength.
     **************************************************************/
    class Cube : public sf::Drawable
    {
    private:

        f32       m_sidelength;
        f32       m_tilt;
        glm::vec3 m_center;
        glm::vec3 m_front;

        ng::GlVertexArray  m_vao;
        ng::GlVertexBuffer m_vbo;
        ng::GlIndexBuffer  m_ebo;

        static constexpr std::array<u32, 36> __getDrawingIndices();
        std::array<ng::Vertex, 8> __calcVertices() const;

    public:

        Cube(f32 sidelength = 1.0,
             f32 tilt       = 0.0,
             const glm::vec3& center = glm::vec3(0, 0, 0), 
             const glm::vec3& front  = glm::vec3(1, 0, 0))
        : m_sidelength(sidelength),
          m_tilt(tilt),
          m_center(center), 
          m_front(glm::normalize(front)),

          m_vbo(GL_STATIC_DRAW),
          m_ebo(GL_STATIC_DRAW)
        { }

              float& sidelength()       { return m_sidelength; }
        const float& sidelength() const { return m_sidelength; }

              glm::vec3& center()       { return m_center; }
        const glm::vec3& center() const { return m_center; }

              glm::vec3& front()       { return m_front; }
        const glm::vec3& front() const { return m_front; }


        void update();

        void draw(sf::RenderTarget& target, 
                  sf::RenderStates states = sf::RenderStates::Default
        ) const;
    };

    inline sf::Vector3f convert(const glm::vec3& v) {
        return sf::Vector3f(v.x, v.y, v.z);
    }
}