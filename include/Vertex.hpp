
#pragma once

/**************************************************************
 * @file   Vertex.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  3D-Vertex format and layout used
 *         in the shaders.
 **************************************************************/

#include <Gl.hpp>
#include <array>

namespace ng
{

    /**************************************************************
     * @brief custom vertex-type with corresponding
     *        layout for vertex-shader.
     **************************************************************/

    struct Vertex
    {  
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 tex;
    };

    struct VertexLayout 
    {
        using vertex_type            = ng::Vertex;
        static constexpr size_t size = 3;
        
        static constexpr 
        std::array<GlVertexAttributeLayout, size> getLayout() {
            return {
                (GlVertexAttributeLayout) { 
                    .count  = 3, 
                    .type   = GL_FLOAT, 
                    .offset = offsetof(ng::Vertex, pos) 
                },
                (GlVertexAttributeLayout) { 
                    .count  = 3, 
                    .type   = GL_FLOAT, 
                    .offset = offsetof(ng::Vertex, color) 
                },
                (GlVertexAttributeLayout) {
                    .count  = 2,
                    .type   = GL_FLOAT,
                    .offset = offsetof(ng::Vertex, tex)
                }
            };
        }
    };

}