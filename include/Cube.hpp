
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
    class Cube
    {
    private:

        f32       m_sidelength;
        f32       m_tilt;
        glm::vec3 m_center;
        glm::vec3 m_front;

        std::array<ng::Vertex, 8> _M_calc_vertices() const;

    public:

        ng::GlVertexBuffer<ng::VertexLayout> m_gl_vbo;

        static
        constexpr std::array<u32, 36> getDrawingIndices()
        {
        return {
            /* front-face */
            0, 1, 2,
            0, 2, 3,
            /* right-face */
            0, 4, 5,
            0, 5, 1,
            /* left-face */
            3, 7, 6,
            3, 6, 2,
            /* back-face */
            7, 4, 5,
            7, 5, 6,
            /* top-face */
            3, 7, 4,
            3, 4, 0,
            /* bottom-face */
            2, 6, 5,
            2, 5, 1
        };
}

        Cube(GlVertexArray<ng::VertexLayout>& glVao,
             f32 sidelength = 1.0,
             f32 tilt       = 0.0,
             const glm::vec3& center = glm::vec3(0, 0, 0), 
             const glm::vec3& front  = glm::vec3(1, 0, 0))
            : m_sidelength(sidelength)
            , m_tilt(tilt)
            , m_center(center)
            , m_front(glm::normalize(front))
            , m_gl_vbo(glVao, GL_STATIC_DRAW)
        { }

              f32& sidelength()       { return m_sidelength; }
        const f32& sidelength() const { return m_sidelength; }

              glm::vec3& center()       { return m_center; }
        const glm::vec3& center() const { return m_center; }

              glm::vec3& front()       { return m_front; }
        const glm::vec3& front() const { return m_front; }

        void update();
    };

    /**************************************************************
     * @brief shares one EBO and VAO for multiple Cubes, which
     *        all have their respective VBO.
     **************************************************************/

    class CubeArray : public sf::Drawable
    {
    public:

        CubeArray()
            : m_gl_ebo(GL_STATIC_DRAW)
            , m_cubes({ Cube{m_gl_vao}, Cube{m_gl_vao} }) // copy-elision here (Cube is noncopyable)
        {
            std::array<u32, 36> indices = ng::Cube::getDrawingIndices();
            m_gl_vao.bind();
            m_gl_ebo.buffer(indices.data(), indices.size());
        }

        ng::GlVertexArray<ng::VertexLayout> m_gl_vao;
        ng::GlIndexBuffer m_gl_ebo;

        std::array<Cube, 2> m_cubes;

        void update();

        void draw(sf::RenderTarget& target,
                  sf::RenderStates states = sf::RenderStates::Default) const;
    };

    class RubiksCube : public CubeArray
    {
    public: 

        RubiksCube(const glm::vec3& pos = glm::vec3(0, 0, 0));

    };
}