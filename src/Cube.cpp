
#include <Cube.hpp>


std::array<ng::Vertex, 8> ng::Cube::__calcVertices() const
{
    /* front() is assumed to be normalized */
    glm::vec3 normal_1 = glm::normalize(glm::cross(front(), { 0.0, 0.0, 1.0 }));
    glm::vec3 normal_2 = glm::normalize(glm::cross(front(), normal_1));
    /* 8 outer vertices */
    /* 4 front-vertices, clockwise */
    /* 4 back-vertices */
    return {
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() + normal_1 + normal_2), { 1.0, 0.0, 0.0 }, { 1.0, 1.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() + normal_1 - normal_2), { 0.0, 1.0, 0.0 }, { 1.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() - normal_1 - normal_2), { 1.0, 0.0, 1.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() - normal_1 + normal_2), { 1.0, 1.0, 0.0 }, { 0.0, 1.0 } },

        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() + normal_1 + normal_2), { 1.0, 0.0, 0.0 }, { 1.0, 1.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() + normal_1 - normal_2), { 0.0, 1.0, 0.0 }, { 1.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() - normal_1 - normal_2), { 1.0, 0.0, 1.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() - normal_1 + normal_2), { 0.0, 1.0, 0.0 }, { 0.0, 1.0 } },
    };
}


void ng::Cube::update()
{
    std::array<Vertex, 8> vertices = __calcVertices();
    m_gl_vbo.buffer(vertices.data(), vertices.size());
}   


void ng::CubeArray::update() {
    if (m_cubes.empty())
    { return; }

    for (Cube& cube : m_cubes) 
    { cube.update(); }
}


void ng::CubeArray::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    /* 36 indices per cube */
    m_gl_vao.bind();
    m_gl_ebo.bind();
    for (const Cube& cube : m_cubes)
    { 
        cube.m_gl_vbo.bind();
        m_gl_vao.draw(36, true);
        cube.m_gl_vbo.unbind();
    }
    m_gl_vao.unbind();
    m_gl_ebo.unbind();
}


ng::RubiksCube::RubiksCube(const glm::vec3& pos) {
    m_cubes[0].center() = glm::vec3(1, 1, 1); 
    m_cubes[1].center() = glm::vec3(0, 1, 2);
}