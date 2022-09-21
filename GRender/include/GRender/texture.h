#pragma once

#include "core.h"

namespace GRender {
class Shader;

namespace texture {
enum class Format : uint8_t {
    NONE,
    RGBA8,
    INTEGER,
    UNSIGNED_INTEGER,
    FLOAT
};

enum class Filter : uint8_t {
    NEAREST,
    LINEAR
};

enum class Wrap : uint8_t {
    REPEAT,     // Uses decimals digits to sample
    MIRRORED,   // 1 - repeat
    BORDER      // Clamps between 0 and 1
};

struct Specification {
    Format fmt = Format::RGBA8;

    struct {
        Filter min = Filter::LINEAR;
        Filter mag = Filter::NEAREST;
    } filter;

    struct {
        Wrap x = Wrap::BORDER;
        Wrap y = Wrap::BORDER;
    } wrap;
};
} // namespace texture

class  Texture {
    using Specification = texture::Specification;
public:
    Texture(const glm::uvec2& size, const texture::Specification& spec = texture::Specification(), const void* data = nullptr);
    Texture(void) = default;
    ~Texture(void);

    // We should not copy GPU data
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    // But we can move IDs around
    Texture(Texture&&) noexcept;
    Texture& operator=(Texture&&) noexcept;
    
    uint32_t id(void) const { return m_TexID;  }
    glm::uvec2 size(void) const { return m_Size; }
    Specification specification(void) const { return m_Spec;  }


    void bind(const Shader& shader, uint32_t slot = 0) const;
    void update(const void* data);
    void resize(const glm::uvec2& size);

    operator bool() const { return m_TexID > 0; }
private:
    uint32_t m_TexID = 0;
    glm::uvec2 m_Size = { 0, 0 };
    Specification m_Spec;
};

// this namespace should be centralized somewhere else at some point
namespace utils {
Texture createTextureFromRGBAFile(const fs::path& filepath, texture::Specification spec = texture::Specification());
}

} // namespace GRender