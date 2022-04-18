#include "quad.h"

namespace GRender::quad {

Quad::Quad(uint32_t numQuads) : maxVertices(4 * numQuads) {
    glad_glGenVertexArrays(1, &vao);
    glad_glBindVertexArray(vao);

    // Copying buffer to gpu
    glad_glGenBuffers(1, &vtxBuffer);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vtxBuffer);
    glad_glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // layout for buffer
    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, pos));
    glad_glEnableVertexAttribArray(0);

    glad_glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, color));
    glad_glEnableVertexAttribArray(1);

    glad_glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texCoord));
    glad_glEnableVertexAttribArray(2);

    glad_glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texID));
    glad_glEnableVertexAttribArray(3);

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

    glad_glGenBuffers(1, &idxBuffer);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuffer);
    glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, vID.size() * sizeof(uint32_t), vID.data(), GL_STATIC_DRAW);

    // Allocating memory for vertex buffer
    vertices.reserve(maxVertices);
}

Quad::~Quad(void) {
    glad_glDeleteBuffers(1, &idxBuffer);
    glad_glDeleteBuffers(1, &vtxBuffer);
    glad_glDeleteVertexArrays(1, &vao);
    
    idxBuffer = vtxBuffer = vao = 0;
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

void Quad::draw(const Specs& spec) {
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

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), {spec.size.x, spec.size.y, 1.0f});
    glm::mat4 rotate = glm::rotate(scale, spec.angle, {0.0f, 0.0f, 1.0f});
    glm::mat4 translate = glm::translate(rotate, spec.position);

    for (uint32_t k = 0; k < 4; k++) {
        glm::vec4 vec = translate * pos[k];
        vertices.emplace_back(Vertex{{ vec.x, vec.y, vec.z }, spec.color, tCoord[k], float(spec.texID) });
    }
}

void Quad::submit(void) {
    glad_glBindVertexArray(vao);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vtxBuffer);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuffer);

    // Let's send our data to the GPU
    glad_glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

    // Issueing the draw call to all quads
    glad_glDrawElements(GL_TRIANGLES, 6 * (vertices.size() >> 2), GL_UNSIGNED_INT, 0);

    // Resetting for next round
    glad_glBindVertexArray(0);
    vertices.clear();
}

} // namespace GRender::quad
