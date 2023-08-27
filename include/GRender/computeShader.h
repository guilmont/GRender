#pragma once

#include "shaderUtils.h"

namespace GRender {
class Texture;
class StorageBuffer;

class ComputeShader {
public:
    ComputeShader(const std::filesystem::path& computePath);
    ComputeShader(void) = default;
    ~ComputeShader();

    // We don't want a shader to be copied
    ComputeShader(const ComputeShader&) = delete;
    ComputeShader& operator=(const ComputeShader&) = delete;

    // But it can be moved no problem
    ComputeShader(ComputeShader&&) noexcept;
    ComputeShader& operator=(ComputeShader&&) noexcept;

    const ComputeShader& bind();

    template<typename TP>
    void setUniform(const std::string& name, const TP& value) const {
        shader::internal::setUniform<TP>(m_ProgramID, name, value);
    }

    void setTexture(const Texture& tex, uint32_t slot = 0) const;
    void setBuffer(const StorageBuffer& buffer, uint32_t slot = 0) const;

    // Launch execution in GPU
    void dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1) const;

private:
    uint32_t m_ProgramID = 0;
};

} // namespace GRender