#include "computeShader.h"
#include "texture.h"
#include "storageBuffer.h"

namespace fs = std::filesystem;

namespace GRender {

static GLenum convertToGLFormat(texture::Format fmt) {
    switch (fmt) {
    case texture::Format::RGBA8:             return GL_RGBA8;
    case texture::Format::RGBA32:            return GL_RGBA32F;
    case texture::Format::INTEGER:           return GL_R32I;
    case texture::Format::UNSIGNED_INTEGER:  return GL_R32UI;
    case texture::Format::FLOAT:             return GL_R32F;
    default:
        ASSERT(false, "Texture format not supported!!");
        return GL_NONE;
    }
}


ComputeShader::ComputeShader(const fs::path& computePath) {
    ASSERT(fs::exists(computePath), "Compute shader not found! => " + computePath.string());
    uint32_t cmp = shader::internal::CreateShader(computePath, GL_COMPUTE_SHADER);

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, cmp);
    glLinkProgram(m_ProgramID);
    glDeleteShader(cmp);
}

ComputeShader::~ComputeShader(void) {
    glDeleteProgram(m_ProgramID);
}

ComputeShader::ComputeShader(ComputeShader&& shader) noexcept {
    std::swap(m_ProgramID, shader.m_ProgramID);
}

ComputeShader& ComputeShader::operator=(ComputeShader&& shader) noexcept {
    if (this != &shader) {
        this->~ComputeShader();
        new (this) ComputeShader(std::move(shader));
    }
    return *this;
}

void ComputeShader::setTexture(const Texture& tex, uint32_t slot) const {
    GLenum fmt = convertToGLFormat(tex.specification().fmt);
    
    tex.bind(slot);
    glBindImageTexture(slot, tex.id(), 0, GL_FALSE, 0, GL_READ_WRITE, fmt);
}

void ComputeShader::setBuffer(const StorageBuffer& buffer, uint32_t slot) const {
    buffer.bind(slot);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, buffer.id());
}

const ComputeShader& ComputeShader::bind() {
    glUseProgram(m_ProgramID);
    return *this;
}

void ComputeShader::dispatch(uint32_t numGroupsX, uint32_t numGroupsY, uint32_t numGroupsZ) const {
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

} // namespace GRender