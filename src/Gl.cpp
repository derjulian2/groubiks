
#include <Gl.hpp>

ng::GlVertexArray::GlVertexArray() {
    glGenVertexArrays(1, &m_gl_handle);
}


ng::GlVertexArray::~GlVertexArray() {
    if (!m_gl_handle)
    { return; }
    glDeleteVertexArrays(1, &m_gl_handle);
}


void ng::GlVertexArray::bind() const {
    glBindVertexArray(m_gl_handle);
}


void ng::GlVertexArray::unbind() {
    glBindVertexArray(0);
}


void ng::GlVertexArray::draw(u32 count, bool indexed, GLenum mode) const {
    if (indexed) {
        glDrawElements(mode, count, toGlType<GlIndexBuffer::index_type>(), 0);
    }
    else {
        glDrawArrays(mode, 0, count);
    }
}


ng::GlBuffer::GlBuffer(GLuint glDrawMode) : m_gl_mode(glDrawMode) {
    glGenBuffers(1, &m_gl_handle);
}


ng::GlBuffer::~GlBuffer() {
    if (!m_gl_handle) 
    { return; }
    glDeleteBuffers(1, &m_gl_handle);
}


void ng::GlBuffer::__bind(GLenum target) const {
    if (!m_gl_handle)
    { throw std::runtime_error("invalid GL-handle"); }
    glBindBuffer(target, m_gl_handle);
}


void ng::GlBuffer::__unbind(GLenum target) {
    glBindBuffer(target, 0);
}


void ng::GlVertexBuffer::bind() const {
    __bind(GL_ARRAY_BUFFER);
}


void ng::GlVertexBuffer::unbind() const {
    __unbind(GL_ARRAY_BUFFER);
}


void ng::GlIndexBuffer::bind() const {
    __bind(GL_ELEMENT_ARRAY_BUFFER);
}


void ng::GlIndexBuffer::unbind() const {
    __unbind(GL_ELEMENT_ARRAY_BUFFER);
}


void ng::GlIndexBuffer::buffer(const index_type* pData, size_t idxCount) const {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(index_type) * idxCount,
        pData,
        drawMode()
    );
}