
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
    using GlIndex  = GLuint;

    template <typename T>
    constexpr GLenum toGlType() {
        if      constexpr (std::same_as<T, f32>)       return GL_FLOAT;
        else if constexpr (std::same_as<T, f64>)       return GL_DOUBLE;
        else if constexpr (std::same_as<T, u32>)       return GL_UNSIGNED_INT;
        else static_assert(false, "unsupported GL-type");
    }

    static inline
    bool isGlLoaded()
    { return GLAD_GL_VERSION_4_3; } // 4.3 required

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
     * @brief basic OpenGL-resource interface.
     **************************************************************/
    class GlResource
    {
    protected:
        GlHandle m_gl_handle;

    public:
        GlResource() 
            : m_gl_handle(0) 
        {
            if (!isGlLoaded())
            { throw std::runtime_error("cannot create GlResource: gl-function-pointers are not loaded"); }
        }

        GlResource(const GlResource&) = delete;
        GlResource& operator=(const GlResource&) = delete;
        GlResource(GlResource&& other) { std::swap(this->m_gl_handle, other.m_gl_handle); }
        GlResource& operator=(GlResource&& other) { std::swap(this->m_gl_handle, other.m_gl_handle); return *this; }

              GlHandle& getNativeHandle()       { return m_gl_handle; }
        const GlHandle& getNativeHandle() const { return m_gl_handle; }
    };

    /**************************************************************
     * @brief OpenGL-vertex-array-object (VAO).
     *        switches to indexed-drawing if
     *        a single EBO gets added to it.
     **************************************************************/
    template <typename Layout>
            requires GlVertexLayout<Layout>
    class GlVertexArray : public GlResource
    {
    protected:
        GLuint m_gl_binding_index;

    public:
        using vertex_type = typename Layout::vertex_type;

        GlVertexArray(GLuint glBindingIndex = 0)
            : m_gl_binding_index(glBindingIndex) {
            glGenVertexArrays(1, &m_gl_handle);
            setLayout();
        }

        ~GlVertexArray() {
            if (!m_gl_handle)
            { return; }
            glDeleteVertexArrays(1, &m_gl_handle);
        }

        GLuint bindingIndex() const { return m_gl_binding_index; }

        void setLayout() {
            bind();
            std::array<GlVertexAttributeLayout, Layout::size> attributes = Layout::getLayout();
            for (u32 i = 0; i < attributes.size(); ++i) {
                GlVertexAttributeLayout& layout = attributes[i];
                glEnableVertexAttribArray(i);
                glVertexAttribFormat(i, 
                    layout.count, 
                    layout.type, 
                    GL_FALSE,
                    layout.offset
                );
                glVertexAttribBinding(i, m_gl_binding_index);
            }
        }

        void bind() const {
            glBindVertexArray(m_gl_handle);
        }

        static void unbind() {
            glBindVertexArray(0);
        }

        void draw(u32 count, bool indexed, GLenum mode = GL_TRIANGLES) const {
            if (indexed) {
                glDrawElements(mode, count, toGlType<GlIndex>(), 0);
            }
            else {
                glDrawArrays(mode, 0, count);
            }
        }
    };

    /**************************************************************
     * @brief OpenGL-buffer interface.
     **************************************************************/
    class GlBuffer : public GlResource
    {
    protected:
        GLuint m_gl_mode;

    public:
        GlBuffer(GLuint glDrawMode);
        ~GlBuffer();

        GLuint drawMode() const { return m_gl_mode; }
    };

    /**************************************************************
     * @brief OpenGL-vertex-buffer-object (VBO). requires vertex-layout.
     **************************************************************/
    template <typename Layout>
            requires GlVertexLayout<Layout>
    class GlVertexBuffer : public GlBuffer
    {
    protected:
        GlVertexArray<Layout>& m_gl_vao;  

    public:
        using vertex_type = typename Layout::vertex_type;
        
        GlVertexBuffer(GlVertexArray<Layout>& glVao, GLuint glDrawMode)
            : GlBuffer(glDrawMode), m_gl_vao(glVao)
        { }
        
        void bind() const {
            glBindVertexBuffer(m_gl_vao.bindingIndex(), m_gl_handle, 0, sizeof(vertex_type));
        }

        void unbind() const {
            glBindVertexBuffer(m_gl_vao.bindingIndex(), 0, 0, 0);
        }

        void buffer(const vertex_type* pData, size_t vertexCount) const {
            bind();
            glNamedBufferData(m_gl_handle,
                sizeof(vertex_type) * vertexCount,
                pData,
                drawMode()
            );
        }

    };

    /**************************************************************
     * @brief OpenGL-element-buffer-object (EBO).
     **************************************************************/
    class GlIndexBuffer : public GlBuffer
    {
    public:
        using index_type = GlIndex;

        using GlBuffer::GlBuffer;

        void bind() const;
        void unbind() const;

        void buffer(const index_type* pData, size_t idxCount) const;

    };

}
