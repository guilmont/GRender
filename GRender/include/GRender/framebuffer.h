#pragma once

#include "core.h"

#include "GRender/texture.h"

namespace GRender {

class  Framebuffer {
public:
    Framebuffer(const glm::uvec2& size, const std::vector<texture::Specification>& vSpecs, bool createDepthBuf = false);
    Framebuffer(const glm::uvec2& size, const texture::Specification& spec = texture::Specification(), bool createDepthBuf = false);
    Framebuffer(void) = default;
    ~Framebuffer(void);

        // To facilitate actions related to present framebuffer
    bool isHovered = false;

    // We don't want a framebuffer to be copied
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    // But it can be moved no problem
    Framebuffer(Framebuffer&& fBuffer) noexcept;
    Framebuffer& operator=(Framebuffer&& fBuffer) noexcept;

    void bind(void);
    void unbind(void);

    void resize(const glm::uvec2& size);

    const Texture& getTexture(uint32_t id = 0) const;
    glm::uvec2 size(void) const { return m_Size; }
    
    void setPosition(float x, float y) { m_Position = { x, y }; }
    const glm::uvec2& getPosition(void) const { return m_Position; }

    operator bool() const { return m_BufferID > 0; }

private:
    bool m_HasDepthBuffer = false;
    uint32_t m_BufferID = 0, m_DepthID = 0;
    std::vector<Texture> m_Textures;

    glm::uvec2 m_Size = { 1, 1 };
    glm::uvec2 m_Position = { 0, 0 };

};

} // namespace GRender