#pragma once

#include "GRender/core.h"

namespace GRender {

struct Vertex {
    glm::vec3 position, normal;
    glm::vec2 texCoord;
};

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

    void submit(const glm::vec3& position, const glm::vec3& color = {1.0f, 1.0f, 1.0f});
    void draw(void);

protected:
    void initialize(const std::vector<Vertex>& vtxBuffer,
                    const std::vector<glm::uvec3>& idxBuffer);

private:
    uint32_t m_MaxNumber;
    uint32_t m_VAO, m_VTX, m_IDX;
    uint32_t m_Position, m_Color;

    GLsizei m_NumIndices = 0;
    std::vector<glm::vec3> m_VecPosition, m_VecColor;
};

} //namespace GRender
