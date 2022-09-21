#pragma once

#include "GRender/core.h"

namespace GRender {

namespace object {

struct Vertex {
    glm::vec3 position, normal;
    glm::vec2 texCoord;
};

struct Specification {
    glm::vec3 position{1.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
    glm::vec4 color{1.0f};
    int32_t textureID = -1;  // no texture
};

}; // namespace object


class Object {
public:
    Object(void) = default;
    Object(uint32_t maxNumber);
    virtual ~Object(void);

    // Allowing to move GPU data around
    Object(Object&&) noexcept;
    Object& operator=(Object&&) noexcept;

    // We don't want to copy GPU data
    Object(const Object&) noexcept = delete;
    Object& operator=(const Object*) noexcept = delete;

    void submit(const object::Specification& specs);
    void draw(void);

protected:
    void initialize(const std::vector<object::Vertex>& vtxBuffer,
                    const std::vector<glm::uvec3>& idxBuffer);

private:
    uint32_t m_MaxNumber = 0;
    uint32_t m_VAO = 0, m_VTX = 0, m_IDX = 0;
    uint32_t m_POS = 0, m_ROT = 0, m_SCL = 0, m_CLR = 0, m_TEX = 0;

    GLsizei m_NumIndices = 0;
    std::vector<glm::vec3> m_Position, m_Rotation, m_Scale;
    std::vector<glm::vec4> m_Color;
    std::vector<int32_t> m_Texture;
};

} //namespace GRender
