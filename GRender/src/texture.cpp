#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fs = std::filesystem;

namespace GRender {

Texture::Texture(uint32_t width, uint32_t height) : size(width, height) {
}

Texture::~Texture(void) {
    glDeleteTextures(1, &texID);
}

Texture::Texture(Texture&& tex) noexcept {
    std::swap(texID, tex.texID);
    std::swap(size, tex.size);
}

Texture& Texture::operator=(Texture&& tex) noexcept {
    if (this != &tex) {
        new(this) Texture(std::move(tex));
    }
    return *this;
}

glm::uvec2 Texture::getSize() const {
    return size;
}

void Texture::bind(uint32_t slot) const {
    ASSERT(slot < 32, "Maximum gpu texture slot exceeded");
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texID);
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TextureRGBA  //////////////////////////////////////////////

TextureRGBA::TextureRGBA(uint32_t width, uint32_t height, const uint32_t* data) : Texture(width, height) {
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureRGBA::TextureRGBA(const fs::path& path) {
    // We will will sbt_image to load image from file
    // It supports most of popular formats: PNG, JPG, TIF, ...
    // RGB will be converted into RGBA automatically
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 4);

    new(this) TextureRGBA(uint32_t(width), uint32_t(height), reinterpret_cast<const uint32_t *>(data));
    stbi_image_free(data);
}

void TextureRGBA::update(const uint32_t* data) {
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_INT, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// TextureFloat //////////////////////////////////////////////

TextureFloat::TextureFloat(uint32_t width, uint32_t height, const float* data) : Texture(width, height) {
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_2D, 0);
} 

void TextureFloat::update(const float* data) {
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RED, GL_FLOAT, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace GRender