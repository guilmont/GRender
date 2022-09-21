#include "quad.h"

namespace GRender {
using namespace quad;

Quad::Quad(uint32_t numQuads) : maxVertices(4 * numQuads) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Copying buffer to gpu
    glGenBuffers(1, &vtxBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vtxBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // layout for buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texID));
    glEnableVertexAttribArray(3);
    
    // Preparing index array
    vID.resize(6 * uint64_t(numQuads));
    for (uint32_t k = 0; k < numQuads; k++)
    {
        vID[6 * uint64_t(k) + 0] = 4 * k + 0;
        vID[6 * uint64_t(k) + 1] = 4 * k + 1;
        vID[6 * uint64_t(k) + 2] = 4 * k + 2;
        vID[6 * uint64_t(k) + 3] = 4 * k + 0;
        vID[6 * uint64_t(k) + 4] = 4 * k + 2;
        vID[6 * uint64_t(k) + 5] = 4 * k + 3;
    }

    glGenBuffers(1, &idxBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vID.size() * sizeof(uint32_t), vID.data(), GL_STATIC_DRAW);

    // Allocating memory for vertex buffer
    vertices.reserve(maxVertices);
}

Quad::~Quad(void) {
    glDeleteBuffers(1, &idxBuffer);
    glDeleteBuffers(1, &vtxBuffer);
    glDeleteVertexArrays(1, &vao);
    
    idxBuffer = vtxBuffer = vao = 0;
    vID.clear();
    vertices.clear();
}

Quad::Quad(Quad&& rhs) noexcept {
    std::swap(vao, rhs.vao);
    std::swap(idxBuffer, rhs.idxBuffer);
    std::swap(vtxBuffer, rhs.vtxBuffer);
    std::swap(maxVertices, rhs.maxVertices);

    // moving vectors
    vID.swap(rhs.vID);
    vertices.swap(rhs.vertices);
}   

Quad& Quad::operator=(Quad&& rhs) noexcept {
    if (&rhs != this) {
        new(this) Quad(std::move(rhs));
    }
    return *this;
}

void Quad::submit(const Specification& spec) {
    ASSERT(maxVertices > 0, "Quad class was not initialized");
    ASSERT(vertices.size() < maxVertices, "Quad class maximum number of vertices was exceeded");

    glm::vec4 pos[] = {{-0.5f, -0.5f, 0.0f, 1.0f},
                       {+0.5f, -0.5f, 0.0f, 1.0f},
                       {+0.5f, +0.5f, 0.0f, 1.0f},
                       {-0.5f, +0.5f, 0.0f, 1.0f}};

    glm::vec2 tCoord[] = {{spec.texCoord[0], spec.texCoord[1]},
                          {spec.texCoord[2], spec.texCoord[1]},
                          {spec.texCoord[2], spec.texCoord[3]},
                          {spec.texCoord[0], spec.texCoord[3]}};

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), spec.position);
    transform = glm::rotate(transform, spec.angle, {0.0f, 0.0f, 1.0f});
    transform = glm::scale(transform, {spec.size.x, spec.size.y, 1.0f});

    for (uint32_t k = 0; k < 4; k++) {
        glm::vec4 vec = transform * pos[k];
        vertices.emplace_back(Vertex{{ vec.x, vec.y, vec.z }, spec.color, tCoord[k], spec.texID });
    }
}

void Quad::draw(void) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vtxBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuffer);

    // Let's send our data to the GPU
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

    // Issueing the draw call to all quads
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(6 * (vertices.size() >> 2)), GL_UNSIGNED_INT, 0);

    // Resetting for next round
    glBindVertexArray(0);
    vertices.clear();
}

} // namespace GRender::quad
