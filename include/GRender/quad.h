#pragma once

#include <vector>
#include "core.h"

#include "shader.h"
#include "texture.h"

namespace GRender {
    
namespace quad {
struct Specification {
    glm::vec4 texCoord = { 0.0f, 0.0f, 1.0f, 1.0f };
    glm::vec4 color    = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec3 position = { 0.0f, 0.0f, 0.0f }; // center of mass
    glm::vec2 size     = { 1.0f, 1.0f };
    float angle        = 0.0f;
    
    Texture* texture = nullptr;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
    int texID;
};
} // namespace quad

class Quad {
public:
    Quad(void) = default;
    Quad(uint32_t numQuads);
    ~Quad(void);

    // We don't want to copy GPU related data
    Quad(const Quad&) = delete;
    Quad& operator=(const Quad&) = delete;
    // It is fine to move it around
    Quad(Quad&&) noexcept;
    Quad& operator=(Quad&&) noexcept;

    // Insert a quad into the buffer for drawing
    void submit(const quad::Specification& spec = quad::Specification());
    // Draws all quads in buffer at once. Depending on camera used, a view matrix shall be provided
    void draw(const glm::mat4& viewMatrix);

private:
    uint32_t
        vao = 0,          // Vertex array object
        vtxBuffer = 0,
        idxBuffer = 0,
        maxVertices = 0;

    std::vector<uint32_t> vID;
    std::vector<quad::Vertex> vertices;
    std::unordered_map<Texture*, int32_t> m_TextureMap;

    // A single shader instance for all quad objects
    static std::unique_ptr<Shader> m_Shader;

};

} // namespace GRender::quad