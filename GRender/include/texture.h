#pragma once

#include <unordered_map>

#include "core.h"

namespace GRender {

class  Texture {
public:
    Texture(uint32_t width, uint32_t height);
    Texture(void) = default;
    virtual ~Texture(void);

    // We should not copy GPU data
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    // But we can move IDs around
    Texture(Texture&&) noexcept;
    Texture& operator=(Texture&&) noexcept;
    
    glm::uvec2 getSize(void) const;
    void bind(uint32_t slot = 0) const;

    uint32_t getID(void) const;

protected:
    uint32_t texID = 0;
    glm::uvec2 size = {0, 0};
};

///////////////////////////////////////////////////////////

struct TextureRGBA : public Texture {
    TextureRGBA(uint32_t width, uint32_t height, const uint32_t* data = nullptr);
    TextureRGBA(const std::filesystem::path& path);
    TextureRGBA(void) = default;
    void update(const uint32_t* data);
};


struct TextureFloat: public Texture {
    TextureFloat(uint32_t width, uint32_t height, const float* data = nullptr);
    TextureFloat(void) = default;
    void update(const float* data);
};

} // namespace GRender