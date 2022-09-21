#pragma once

#include "core.h"

namespace GRender {
class Texture;

class Shader {
public:
    Shader(const std::filesystem::path& vtxPath, const std::filesystem::path& frgPath);
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

    void setTexture(const Texture& tex, uint32_t slot = 0) const;

    void setInteger(const std::string&, int) const;
    void setFloat(const std::string&, float) const;
    void setVec2f(const std::string&, const float *) const;
    void setVec3f(const std::string&, const float *) const;
    void setVec4f(const std::string&, const float *) const;
    void setMatrix3f(const std::string&, const float *) const;
    void setMatrix4f(const std::string&, const float *) const;

    void setIntArray(const std::string&, const int *, int) const;

    void setFloatArray(const std::string&, const float *, int) const;
    void setVec2fArray(const std::string&, const float *, int) const;
    void setVec3fArray(const std::string&, const float *, int) const;
    void setMat3Array(const std::string&, const float *, int) const;

private:
    uint32_t programID = 0;
};

} // namespace GRender