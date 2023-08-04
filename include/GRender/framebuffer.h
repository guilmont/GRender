#pragma once

#include "core.h"

#include "GRender/texture.h"

namespace GRender {

class  Framebuffer {
public:
    Framebuffer(const glm::uvec2& size, const std::vector<texture::Specification>& vSpecs = {texture::Specification()}, bool createDepthBuf = false);
    Framebuffer(void) = default;
    ~Framebuffer(void);

    // We don't want a framebuffer to be copied
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    // But it can be moved, no problem
    Framebuffer(Framebuffer&& fBuffer) noexcept;
    Framebuffer& operator=(Framebuffer&& fBuffer) noexcept;

    void bind(void) const;
    void unbind(void) const;

    void resize(const glm::uvec2& size);

    const Texture& texture(uint32_t id = 0) const;
    glm::uvec2 size(void) const { return m_Size; }
    
    operator bool() const { return m_BufferID > 0; }

private:
    bool m_HasDepthBuffer = false;
    uint32_t m_BufferID = 0, m_DepthID = 0;
    std::vector<Texture> m_Textures;

    glm::uvec2 m_Size = { 1, 1 };
};

} // namespace GRender