#pragma once

#include "core.h"

namespace GRender
{

    class  Framebuffer
    {
    public:
        Framebuffer(void) = default;
        Framebuffer(uint32_t width, uint32_t height);
        Framebuffer(Framebuffer&& buf) noexcept;

        Framebuffer& operator=(Framebuffer&& buf) noexcept;

        ~Framebuffer(void);

        void bind(void);
        void unbind(void);

        uint32_t getID(void) { return textureID; }

        void setPosition(float x, float y) { position = { x, y }; }
        const glm::vec2& getPosition(void) const { return position; }

        glm::vec2 getSize(void) const { return size; }

    private:
        uint32_t bufferID = 0, textureID = 0, depthID = 0;
        glm::vec2 size = { 1.0f, 1.0f }, position = { 0.0f, 0.0f };

    };

}