#pragma once

#include <vector>
#include "core.h"

namespace GRender::quad {

struct Specs {
    glm::vec3 position = {0.0f, 0.0f, 0.0f}; // center of mass
    glm::vec2 size = {1.0f, 1.0f};
    float angle = 0.0f;
    uint32_t texID = 0;
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 texCoord = {0.0f, 0.0f, 1.0f, 1.0f};
};

struct Vertex {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texID;
};

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


    void draw(const Specs& spec = Specs());
    void submit(void);

private:
    uint32_t
        vao = 0,          // Vertex array object
        vtxBuffer = 0,
        idxBuffer = 0,
        maxVertices = 0; // Maximum number of quads

    std::vector<uint32_t> vID;
    std::vector<Vertex> vertices;

};

} // namespace GRender::quad