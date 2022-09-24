#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GRender {
using namespace texture;

static std::tuple<GLint, GLenum, GLenum> convertToGLFormat(Format fmt) {
    switch (fmt) {
    case Format::RGBA8:             return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
    case Format::INTEGER:           return { GL_R32I,  GL_RED_INTEGER, GL_INT };
    case Format::UNSIGNED_INTEGER:  return { GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT };
    case Format::FLOAT:             return { GL_R32F,  GL_RED, GL_FLOAT };
    default:
        ASSERT(false, "Texture format not supported!!");
        return {0,0,0};
    }
}

static GLint convertToGLFilter(Filter filter) {
    switch (filter) {
    case Filter::LINEAR:  return GL_LINEAR;
    case Filter::NEAREST: return GL_NEAREST;
    default:
        ASSERT(false, "Texture filter not supported!!");
        return 0;
    }
}

static GLint convertToGLWrap(Wrap wrap) {
    switch (wrap) {
    case Wrap::REPEAT:   return GL_REPEAT;
    case Wrap::MIRRORED: return GL_MIRRORED_REPEAT;
    case Wrap::BORDER:   return GL_CLAMP_TO_BORDER;
    default:
        ASSERT(false, "Texture wrap mode not supported!!");
        return 0;
    }
}

Texture::Texture(const glm::uvec2& size, const Specification& spec, const void* data) : m_Size(size), m_Spec(spec) {
    glGenTextures(1, &m_TexID);
    glBindTexture(GL_TEXTURE_2D, m_TexID);

    auto [intFmt, fmt, tp] = convertToGLFormat(spec.fmt);
    glTexImage2D(GL_TEXTURE_2D, 0, intFmt, size.x, size.y, 0, fmt, tp, data);

    // Wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convertToGLWrap(spec.wrap.x));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convertToGLWrap(spec.wrap.y));

    // Min mag filters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convertToGLFilter(spec.filter.min));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convertToGLFilter(spec.filter.mag));

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture(void) {
    glDeleteTextures(1, &m_TexID);
}

Texture::Texture(Texture&& tex) noexcept {
    std::swap(m_TexID, tex.m_TexID);
    std::swap(m_Size, tex.m_Size);
    std::swap(m_Spec, tex.m_Spec);
}

Texture& Texture::operator=(Texture&& tex) noexcept {
    if (this != &tex) {
        this->~Texture();
        new(this) Texture(std::move(tex));
    }
    return *this;
}

void Texture::bind(uint32_t slot) const {
    ASSERT(*this, "Texture not initialized!!");
    ASSERT(slot < 32, "Maximum gpu texture slot exceeded");
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TexID);
}


void Texture::update(const void* data) {
    glBindTexture(GL_TEXTURE_2D, m_TexID);

    auto [intFmt, fmt, tp] = convertToGLFormat(m_Spec.fmt);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Size.x, m_Size.y, fmt, tp, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::resize(const glm::uvec2& size) {
    auto locSpec = m_Spec;
    new (this) Texture(size, locSpec);
}

namespace utils {

Texture createTextureFromRGBAFile(const fs::path& filepath, Specification spec) {
    ASSERT(fs::is_regular_file(filepath), "File not found: " + filepath.string());
    ASSERT(spec.fmt == Format::RGBA8, "Expected RGBA8 texture format specification!!");

    // We will will sbt_image to load image from file
    // It supports most of popular formats: PNG, JPG, TIF, ...
    // RGB will be converted into RGBA automatically

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    uint8_t* data = stbi_load(filepath.string().c_str(), &width, &height, &nrChannels, 4);

    Texture texture({ width, height }, spec, data);
    stbi_image_free(data);

    return texture;
}

} // namespace utils


} // namespace GRender