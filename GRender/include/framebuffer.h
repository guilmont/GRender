#pragma once

#include "core.h"

namespace GRender {

class  Framebuffer {
public:
    Framebuffer(void) = default;
    Framebuffer(const glm::uvec2& size);
    Framebuffer(uint32_t width, uint32_t height);
    ~Framebuffer(void);

    // We don't want a framebuffer to be copied
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    // But it can be moved no problem
    Framebuffer(Framebuffer&& fBuffer) noexcept;
    Framebuffer& operator=(Framebuffer&& fBuffer) noexcept;

    void bind(void);
    void unbind(void);

    uint32_t getID(void) { return textureID; }

    void setPosition(float x, float y) { position = { x, y }; }
    const glm::vec2& getPosition(void) const { return position; }

    glm::uvec2 getSize(void) const { return size; }

private:
    uint32_t bufferID = 0, textureID = 0, depthID = 0;
    glm::uvec2 size = { 1, 1 }, position = { 0, 0 };

};

} // namespace GRender