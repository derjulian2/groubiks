
#pragma once

/**************************************************************
 * @file   Gl.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  OpenGL-resource RAII-wrappers.
 **************************************************************/

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Types.hpp>
#include <Logging.hpp>

#include <concepts>
#include <stdexcept>

namespace ng
{
    /**************************************************************
     * @brief OpenGL-interface code handling.
     **************************************************************/

    using GlHandle = GLuint;

    template <typename T>
    constexpr GLenum toGlType() {
        if      constexpr (std::same_as<T, f32>)       return GL_FLOAT;
        else if constexpr (std::same_as<T, f64>)       return GL_DOUBLE;
        else if constexpr (std::same_as<T, u32>)       return GL_UNSIGNED_INT;
        else static_assert(false, "unsupported GL-type");
    }

    /**************************************************************
     * @brief basic OpenGL-resource interface.
     **************************************************************/
    class GlResource
    {
    protected:
        GlHandle m_gl_handle;

    public:
        GlResource() : m_gl_handle(0) { }
        GlResource(const GlResource&) = delete;
        GlResource(GlResource&& other) { std::swap(this->m_gl_handle, other.m_gl_handle); }

        GlHandle getNativeHandle() const { return m_gl_handle; }
    };

    /**************************************************************
     * @brief OpenGL-vertex-array-object (VAO).
     *        switches to indexed-drawing if
     *        a single EBO gets added to it.
     **************************************************************/
    class GlVertexArray : public GlResource
    {
    public:
        GlVertexArray();
        ~GlVertexArray();

        void bind() const;
        static void unbind();

        void draw(u32 count, bool indexed, GLenum mode = GL_TRIANGLES) const;
    };

    /**************************************************************
     * @brief OpenGL-buffer interface.
     **************************************************************/
    class GlBuffer : public GlResource
    {
    protected:
        GLuint m_gl_mode;

        void __bind(GLenum target) const;
        static void __unbind(GLenum target);

    public:
        GlBuffer(GLuint glDrawMode);
        ~GlBuffer();

        GLuint drawMode() const { return m_gl_mode; }

        virtual void bind() const = 0;
        virtual void unbind() const = 0;
    };

    struct GlVertexAttributeLayout
    {
        GLuint  count;
        GLenum  type;
        GLsizei offset;
    };

    template <typename T>
    concept GlVertexLayout = requires(T s) {
        typename T::vertex_type;
        { T::size } -> std::convertible_to<std::size_t>;
        { T::getLayout() } -> std::convertible_to<std::array<GlVertexAttributeLayout, T::size>>;
    };


    /**************************************************************
     * @brief OpenGL-vertex-buffer-object (VBO). requires vertex-layout.
     **************************************************************/
    class GlVertexBuffer : public GlBuffer
    {
    public:
        using GlBuffer::GlBuffer;

        virtual void bind() const override;
        virtual void unbind() const override;

        template <typename Layout>
            requires GlVertexLayout<Layout>
        void buffer(const Layout::vertex_type* pData, size_t vertexCount) const;

    };

    /**************************************************************
     * @brief OpenGL-element-buffer-object (EBO).
     **************************************************************/
    class GlIndexBuffer : public GlBuffer
    {
    public:
        using index_type = u32;

        using GlBuffer::GlBuffer;

        virtual void bind() const override;
        virtual void unbind() const override;

        void buffer(const index_type* pData, size_t idxCount) const;

    };

}


template <typename Layout>
    requires ng::GlVertexLayout<Layout>
void ng::GlVertexBuffer::buffer(const Layout::vertex_type* pData, size_t vertexCount) const {
    log_debug(std::format("buffering {} vertices", vertexCount));
    glBufferData(GL_ARRAY_BUFFER, 
        sizeof(typename Layout::vertex_type) * vertexCount, 
        pData, 
        drawMode()
    );
    std::array<GlVertexAttributeLayout, Layout::size> attributes = Layout::getLayout();
    for (u32 i = 0; i < attributes.size(); ++i) {
        GlVertexAttributeLayout& layout = attributes[i];
        log_debug(std::format("attribute: {}, {}, {}", layout.count, layout.type, layout.offset));
        glVertexAttribPointer(i, 
            layout.count, 
            layout.type, 
            GL_FALSE, 
            sizeof(typename Layout::vertex_type), 
            reinterpret_cast<const void*>(layout.offset)
        );
        glEnableVertexAttribArray(i);
    }
}