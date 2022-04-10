#pragma once

#include <unordered_map>

#include "core.h"

namespace GRender {

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
    Shader& bind(void);

    void setInteger(const std::string&, int);
    void setFloat(const std::string&, float);
    void setVec2f(const std::string&, const float *);
    void setVec3f(const std::string&, const float *);
    void setVec4f(const std::string&, const float *);
    void setMatrix3f(const std::string&, const float *);
    void setMatrix4f(const std::string&, const float *);

    void setIntArray(const std::string&, const int *, int);

    void setFloatArray(const std::string&, const float *, int);
    void setVec2fArray(const std::string&, const float *, int);
    void setVec3fArray(const std::string&, const float *, int);
    void setMat3Array(const std::string&, const float *, int);

private:
    uint32_t programID = 0;
};

} // namespace GRender