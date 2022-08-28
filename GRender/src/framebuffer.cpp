#include "framebuffer.h"

#include "texture.h"

namespace GRender {
using TexSpec = texture::Specification;

Framebuffer::Framebuffer(const glm::uvec2& size, const TexSpec& spec, bool createDepthBuf)
    : Framebuffer(size, std::vector<TexSpec>{ spec }, createDepthBuf) {}

Framebuffer::Framebuffer(const glm::uvec2& size, const std::vector<TexSpec>& vSpecs,  bool createDepthBuf)
    : m_HasDepthBuffer(createDepthBuf), m_Size(size) {

    glGenFramebuffers(1, &m_BufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

    // Creating textures for all color attachments
    std::vector<GLenum> buffers(vSpecs.size());
    for (size_t k = 0; k < vSpecs.size(); k++) {
        const TexSpec& spec = vSpecs[k];
        auto& tex = m_Textures.emplace_back(size, spec);

        buffers[k] = GL_COLOR_ATTACHMENT0 + uint32_t(k);
        glFramebufferTexture2D(GL_FRAMEBUFFER, buffers[k], GL_TEXTURE_2D, tex.id(), 0);
    }

    // Telling OpenGL to draw in all attached buffers
    glDrawBuffers(uint32_t(buffers.size()), buffers.data());

    // If necessary we also create a depth buffer
    if (createDepthBuf) {
        glGenTextures(1, &m_DepthID);
        glBindTexture(GL_TEXTURE_2D, m_DepthID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthID, 0);
    }

    // Testing if it worked properly
    ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!!");

    // binding standard buffer back
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer(void) {
    glDeleteTextures(1, &m_DepthID);
    m_Textures.clear();
    glDeleteFramebuffers(1, &m_BufferID);
}

Framebuffer::Framebuffer(Framebuffer&& fBuffer) noexcept {
    std::swap(m_HasDepthBuffer, fBuffer.m_HasDepthBuffer);
    std::swap(m_BufferID, fBuffer.m_BufferID);
    std::swap(m_DepthID, fBuffer.m_DepthID);
    std::swap(m_Textures, fBuffer.m_Textures);
    std::swap(m_Size, fBuffer.m_Size);
    std::swap(m_Position, fBuffer.m_Position);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& fBuffer) noexcept {
    if (&fBuffer != this) {
        new(this) Framebuffer(std::move(fBuffer));
    }
    return *this;
}

const Texture& Framebuffer::getTexture(uint32_t id) const {
    ASSERT(id < m_Textures.size(), "Texture id overflow!!");
    return m_Textures[id];
}

void Framebuffer::bind(void) {
    ASSERT(*this, "Framebuffer not defined!");
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
    glViewport(0, 0, m_Size.x, m_Size.y);
}

void Framebuffer::unbind(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(const glm::uvec2& size) {
    bool depth = m_HasDepthBuffer;
    std::vector<TexSpec> vec;
    std::transform(m_Textures.begin(), m_Textures.end(), std::back_inserter(vec), [](const Texture& tex) { return tex.specification(); });

    new (this) Framebuffer(size, vec, depth);
}


} // namespace GRender
