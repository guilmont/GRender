#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace GRender
{

    void Texture::terminate(void)
    {
        for (auto& [label, specs] : textures)
            glad_glDeleteTextures(1, &specs.texID);
    }


   void Texture::createRGBA(const std::string& label, uint32_t width, uint32_t height, const uint32_t* data)
   {
        Specification specs;
        specs.type = Type::RGBA;
        specs.width = width;
        specs.height = height;

        glad_glGenTextures(1, &specs.texID);
        glad_glBindTexture(GL_TEXTURE_2D, specs.texID);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // I'll pass a black texture
        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // copying into the data base
        textures[label] = std::move(specs);

        // Unbiding texture
        glad_glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::createRGBA(const std::string& label, const fs::path& path)
    {
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(1);
        uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 4);

        createRGBA(label, uint32_t(width), uint32_t(height), reinterpret_cast<const uint32_t *>(data));

        stbi_image_free(data);
    }

    void Texture::updateRGBA(const std::string& label, const uint32_t* data)
    {
        auto it = textures.find(label);
        assert(it != textures.end());

        const Specification& specs = it->second;
        assert(specs.type == Type::RGBA);

        glad_glBindTexture(GL_TEXTURE_2D, specs.texID);
        glad_glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, specs.width, specs.height, GL_RGBA, GL_UNSIGNED_INT, data);
        glad_glBindTexture(GL_TEXTURE_2D, 0);
    }


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    void Texture::createFloat(const std::string& label, uint32_t width, uint32_t height, const float* data)
    {
        Specification specs;
        specs.type = Type::FLOAT;
        specs.width = width;
        specs.height = height;

        glad_glGenTextures(1, &specs.texID);
        glad_glBindTexture(GL_TEXTURE_2D, specs.texID);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // I'll pass a black texture
        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);

        // copying into the data base
        textures[label] = std::move(specs);

        // Unbiding texture
        glad_glBindTexture(GL_TEXTURE_2D, 0);
    } 

    void Texture::updateFloat(const std::string& label, const float* data)
    {
        auto it = textures.find(label);
        assert(it != textures.end());

        const Specification& specs = it->second;
        assert(specs.type == Type::FLOAT);

        glad_glBindTexture(GL_TEXTURE_2D, specs.texID);
        glad_glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, specs.width, specs.height, GL_RED, GL_FLOAT, data);
        glad_glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::bind(const std::string &label, uint32_t slot) const 
    {
        auto it = textures.find(label);

        assert(it != textures.end());
        assert(slot < 32);

        glad_glActiveTexture(GL_TEXTURE0 + slot); 
        glad_glBindTexture(GL_TEXTURE_2D, (it->second).texID);

    } // bind

    void Texture::erase(const std::string &label)
    {
        auto it = textures.find(label);
        assert(it != textures.end());
        glad_glDeleteTextures(1, &(it->second.texID));
        textures.erase(it);
    }

    Texture::Specification Texture::getSpecification(const std::string& label) const
    {
        auto it = textures.find(label);
        assert(it != textures.end());
        return it->second;
    }

   

}