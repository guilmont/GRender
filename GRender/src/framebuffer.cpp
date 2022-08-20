#include "framebuffer.h"

#include "texture.h"

namespace GRender {
    
Framebuffer::Framebuffer(uint32_t width, uint32_t height, bool createDepthBuf) : m_Size(width, height) {
    glGenFramebuffers(1, &m_BufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Assigning texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);

    if (createDepthBuf) {
        glGenTextures(1, &m_DepthID);
        glBindTexture(GL_TEXTURE_2D, m_DepthID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthID, 0);
    }

    // Testing if it worked properly
    ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!!");

    // binding standard buffer back
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(const glm::uvec2& size, bool createDepthBuf)
    : Framebuffer(size.x, size.y, createDepthBuf) {}


Framebuffer::~Framebuffer(void) {
    glDeleteTextures(1, &m_DepthID);
    glDeleteTextures(1, &m_TextureID);
    glDeleteFramebuffers(1, &m_BufferID);
}

Framebuffer::Framebuffer(Framebuffer&& fBuffer) noexcept {
    std::swap(m_BufferID, fBuffer.m_BufferID);
    std::swap(m_TextureID, fBuffer.m_TextureID);
    std::swap(m_DepthID, fBuffer.m_DepthID);
    std::swap(m_Size, fBuffer.m_Size);
    std::swap(m_Position, fBuffer.m_Position);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fBuffer) noexcept {
    if (&fBuffer != this) {
        new(this) Framebuffer(std::move(fBuffer));
    }
    return *this;
}

void Framebuffer::bind(void) {
    ASSERT(m_BufferID != 0, "Framebuffer not defined!");
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
}

void Framebuffer::unbind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::getColorData(uint32_t* cpuPtr, const glm::uvec2& posZero, const glm::uvec2& size) {
    ASSERT(m_TextureID > 0, "Framebuffer::getColorData -> Color buffer not initialized!");
    ASSERT(posZero.x + size.x < m_Size.x, "Frambuffer::getColorData -> Range exceeds buffer size!");
    ASSERT(posZero.y + size.y < m_Size.y, "Frambuffer::getColorData -> Range exceeds buffer size!");

    glReadBuffer(GL_COLOR_ATTACHMENT0); // In the future, if we may want to extend to more color attachments
    glReadPixels(posZero.x, posZero.y, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, cpuPtr);
}

void Framebuffer::getDepthData(float* cpuPtr, const glm::uvec2& posZero, const glm::uvec2& size) {
    ASSERT(m_DepthID > 0, "Framebuffer::getDepthData -> Depth buffer not initialized!");
    ASSERT(posZero.x + size.x < m_Size.x, "Frambuffer::getDepthData -> Range exceeds buffer size!");
    ASSERT(posZero.y + size.y < m_Size.y, "Frambuffer::getDepthData -> Range exceeds buffer size!");

    glReadBuffer(GL_DEPTH_ATTACHMENT); // In the future, if we may want to extend to more color attachments
    glReadPixels(posZero.x, posZero.y, size.x, size.y, GL_DEPTH_COMPONENT32F, GL_FLOAT, cpuPtr);
}


void Framebuffer::copyTextureData(const Texture& destTexture, const glm::uvec2& posZero, const glm::uvec2& size) {
    ASSERT(m_TextureID > 0, "Framebuffer::getColorData -> Color buffer not initialized!");
    ASSERT(posZero.x + size.x < m_Size.x, "Frambuffer::getColorData -> Range exceeds buffer size!");
    ASSERT(posZero.y + size.y < m_Size.y, "Frambuffer::getColorData -> Range exceeds buffer size!");

    glCopyImageSubData(m_TextureID, GL_TEXTURE_2D, 0, posZero.x, posZero.y, 0, destTexture.getID(), GL_TEXTURE_2D, 0, 0, 0, 0, size.x, size.y, 1);
}

} // namespace GRender