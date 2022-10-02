#pragma once

#include "core.h"

namespace GRender {
class Texture;

class Shader {
public:
    Shader(const std::filesystem::path& vtxPath, const std::filesystem::path& frgPath);
    Shader(const std::filesystem::path& computePath);
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

    // Used to dispatch compute shader programs
    void dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1) const;

    void setTexture(const Texture& tex, uint32_t slot = 0) const;


    template<typename TP>
    void setUniform(const std::string&, const TP&) const;


private:
    enum class Type : uint8_t {
        RENDER = 0,
        COMPUTE = 1,
    };

    uint32_t programID = 0;
    Type type = Type::RENDER;
};

} // namespace GRender