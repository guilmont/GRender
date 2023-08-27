#include "shader.h"
#include "texture.h"


namespace fs = std::filesystem;

namespace GRender {


Shader::Shader(const fs::path& vtxPath, const fs::path& frgPath) {
    ASSERT(fs::exists(vtxPath), "Shader not found! => " + vtxPath.string());
    ASSERT(fs::exists(frgPath), "Shader not found! => " + frgPath.string());

    uint32_t vtx = shader::internal::CreateShader(vtxPath, GL_VERTEX_SHADER);
    uint32_t frg = shader::internal::CreateShader(frgPath, GL_FRAGMENT_SHADER);

    // Create program
    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vtx);
    glAttachShader(m_ProgramID, frg);

    // Link shaders to program
    glLinkProgram(m_ProgramID);

    shader::internal::CheckShaderError(m_ProgramID, GL_LINK_STATUS, true, 
                     "Cannot link shader programs => " + vtxPath.string() + " -- " + frgPath.string());

    glDeleteShader(vtx);
    glDeleteShader(frg);
}

Shader::~Shader(void) {
    glDeleteProgram(m_ProgramID);
}

Shader::Shader(Shader&& shader) noexcept {
    std::swap(m_ProgramID, shader.m_ProgramID);
}

Shader& Shader::operator=(Shader&& shader) noexcept {
    if (this != &shader) {
        this->~Shader();
        new (this) Shader(std::move(shader));
    }
    return *this;
}

const Shader& Shader::bind() {
    glUseProgram(m_ProgramID);
    return *this;
}

void Shader::setTexture(const Texture& tex, uint32_t slot) const {
    const std::string name = "texSampler[" + std::to_string(slot) + "]";
 
    tex.bind(slot);
    int32_t loc = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1i(loc, slot);
}

} // namespace GRender