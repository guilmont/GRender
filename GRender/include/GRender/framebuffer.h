#pragma once

#include "core.h"

namespace GRender {
class Texture;

class  Framebuffer {
public:
    Framebuffer(void) = default;
    Framebuffer(const glm::uvec2& size, bool createDepthBuf = true);
    Framebuffer(uint32_t width, uint32_t height, bool createDepthBuf = true);
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

    uint32_t getID(void) { return m_TextureID; }
    glm::uvec2 getSize(void) const { return m_Size; }
    
    void setPosition(float x, float y) { m_Position = { x, y }; }
    const glm::uvec2& getPosition(void) const { return m_Position; }

    // Retrieving data from buffers
    void getColorData(uint32_t* cpuPtr, const glm::uvec2& posZero, const glm::uvec2& size);
    void getDepthData(float* cpuPtr, const glm::uvec2& posZero, const glm::uvec2& size);

    void copyTextureData(const Texture& destTexture, const glm::uvec2& posZero, const glm::uvec2& size);

private:
    uint32_t m_BufferID = 0, m_TextureID = 0, m_DepthID = 0;
    glm::uvec2 m_Size = { 1, 1 }, m_Position = { 0, 0 };

};

} // namespace GRender