#pragma once

#include <unordered_map>

#include "core.h"

namespace GRender {

class  Texture {
public:
    enum class Type : uint8_t
    {
        RGBA = 0,
        FLOAT = 1
    };

    struct Specification
    {
        Type type;
        uint32_t width, height, texID;
    };

public:
    Texture(void) = default;
    ~Texture(void) = default;

    void reset(void);
        
    void createRGBA(const std::string& label,  uint32_t width, uint32_t height, const uint32_t* data = nullptr);
    void createRGBA(const std::string& label, const std::filesystem::path& path);
    void updateRGBA(const std::string& label, const uint32_t* data);
        
    void createFloat(const std::string& label, uint32_t width, uint32_t height, const float* data = nullptr);
    void updateFloat(const std::string& label, const float* data);
        
    void bind(const std::string& label, uint32_t slot) const;
        
    void erase(const std::string &label);

    Specification getSpecification(const std::string& label) const;

private:
    std::unordered_map<std::string, Specification> textures;
};

} // namespace GRender