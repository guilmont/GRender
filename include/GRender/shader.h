#pragma once

#include "core.h"

#include "shaderUtils.h"

namespace GRender {
class Texture;
class StorageBuffer;

class Shader {
public:
    Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    Shader(void) = default;
    ~Shader(void);

    // We don't want a shader to be copied
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // But it can be moved no problem
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;

    // Return a reference to current object for easier handling
    const Shader& bind(void);

    // A set of uniforms predefined by OpenGL.
    template<typename TP>
    void setUniform(const std::string& name, const TP& value) const {
        shader::internal::setUniform<TP>(m_ProgramID, name, value);
    }

    // Sends texture to GPU at set slot
    void setTexture(const Texture& tex, uint32_t slot = 0) const;

private:
    uint32_t m_ProgramID = 0;
};

} // namespace GRender