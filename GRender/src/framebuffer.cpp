#include "framebuffer.h"

namespace GRender {
    
Framebuffer::Framebuffer(uint32_t width, uint32_t height, bool createDepthBuf) : size(width, height) {
    glGenFramebuffers(1, &bufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Assigning texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    if (createDepthBuf) {
        glGenTextures(1, &depthID);
        glBindTexture(GL_TEXTURE_2D, depthID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);
    }

    // Testing if it worked properly
    ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!!");

    // binding standard buffer back
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
} 

Framebuffer::Framebuffer(const glm::uvec2& size, bool createDepthBuf)
    : Framebuffer(size.x, size.y, createDepthBuf) {}


Framebuffer::~Framebuffer(void) {
    glDeleteTextures(1, &depthID);
    glDeleteTextures(1, &textureID);
    glDeleteFramebuffers(1, &bufferID);
}

Framebuffer::Framebuffer(Framebuffer&& fBuffer) noexcept {
    std::swap(bufferID, fBuffer.bufferID);
    std::swap(textureID, fBuffer.textureID);
    std::swap(depthID, fBuffer.depthID);
    std::swap(size, fBuffer.size);
    std::swap(position, fBuffer.position);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fBuffer) noexcept {
    if (&fBuffer != this) {
        new(this) Framebuffer(std::move(fBuffer));
    }
    return *this;
}

void Framebuffer::bind(void) {
    ASSERT(bufferID != 0, "Framebuffer not defined!");
    glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
    glViewport(0, 0, size.x, size.y);
}

void Framebuffer::unbind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace GRender