
#include <Gl.hpp>


ng::GlBuffer::GlBuffer(GLuint glDrawMode) 
    : m_gl_mode(glDrawMode) {
    glGenBuffers(1, &m_gl_handle);
}


ng::GlBuffer::~GlBuffer() {
    if (!m_gl_handle) 
    { return; }
    glDeleteBuffers(1, &m_gl_handle);
}


void ng::GlIndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_handle);
}


void ng::GlIndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ng::GlIndexBuffer::buffer(const index_type* pData, size_t idxCount) const {
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(index_type) * idxCount,
        pData,
        drawMode()
    );
}
