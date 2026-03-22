
#include <Cube.hpp>

constexpr std::array<ng::u32, 36> ng::Cube::__getDrawingIndices()
{
    return {
        /* front-face */
        0, 4, 3,
        7, 3, 4,
        /* right-face */
        3, 7, 2,
        6, 2, 7,
        /* left-face */
        4, 0, 1,
        5, 4, 1,
        /* back-face */
        6, 5, 1,
        6, 1, 2,
        /* top-face */
        5, 7,4,
        5, 6, 7,
        /* bottom-face */
        0, 3, 2,
        0, 2, 1
    };
}

std::array<ng::Vertex, 8> ng::Cube::__calcVertices() const
{
    const auto to_str = [](const glm::vec3& v) 
    { return std::format("{}, {}, {} ", v.x, v.y, v.z); };
    /* front() is assumed to be normalized */
    glm::vec3 normal_1 = glm::normalize(glm::cross(front(), { 0.0, 0.0, 1.0 }));
    glm::vec3 normal_2 = glm::normalize(glm::cross(front(), normal_1));
    log_debug(std::format("n1={}, n2={}", to_str(normal_1), to_str(normal_2)));
    /* 8 outer vertices */
    /* 4 bottom vertices, clockwise from bottomleft */
    /* 4 top vertices, clockwise from topleft */
    return {
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() + normal_1 + normal_2), { 1.0, 0.0, 0.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() + normal_1 - normal_2), { 0.0, 1.0, 0.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() - normal_1 + normal_2), { 1.0, 0.0, 1.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (front() - normal_1 - normal_2), { 1.0, 1.0, 0.0 }, { 0.0, 0.0 } },

        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() + normal_1 + normal_2), { 1.0, 0.0, 0.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() + normal_1 - normal_2), { 0.0, 1.0, 0.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() - normal_1 + normal_2), { 1.0, 0.0, 1.0 }, { 0.0, 0.0 } },
        (ng::Vertex){ center() + (sidelength()/2.0f) * (-front() - normal_1 - normal_2), { 0.0, 1.0, 0.0 }, { 0.0, 0.0 } },
    };
}

void ng::Cube::update()
{
    const auto to_str = [](const glm::vec3& v) 
    { return std::format("{}, {}, {} ", v.x, v.y, v.z); };

    log_debug(std::format("center: {}, front: {}, sidelen: {}", to_str(center()), to_str(front()), sidelength()));
    std::array<Vertex, 8> vertices = __calcVertices();
    std::array<u32, 36> indices = __getDrawingIndices();

    for (const auto& v : vertices) {
        log_debug(std::format("vertex: {}", to_str(v.pos)));
    }

    m_vao.bind();
    
    m_vbo.bind();
    m_vbo.buffer<VertexLayout>(vertices.data(), vertices.size());
    m_ebo.bind();
    m_ebo.buffer(indices.data(), indices.size());

    log_debug(std::format("buffered {} vertices and {} indices", vertices.size(), indices.size()));
}

void ng::Cube::draw(sf::RenderTarget& target, 
                  sf::RenderStates states) const
{
    m_vao.bind();
    m_vao.draw(36, true);
    m_vao.unbind();
    m_vbo.unbind();
    m_ebo.unbind();
}