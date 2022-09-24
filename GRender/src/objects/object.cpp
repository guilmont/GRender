

#include "GRender/objects/object.h"

namespace GRender {

using Vertex = object::Vertex;
using Specification = object::Specification;

Object::Object(uint32_t maxNumber) : m_MaxNumber(maxNumber) {
    m_Position.reserve(maxNumber);
    m_Rotation.reserve(maxNumber);
    m_Scale.reserve(maxNumber);
    m_Color.reserve(maxNumber);
    m_Texture.reserve(maxNumber);
}

Object::~Object(void) {
    if (m_VAO == 0) { return; } // nothing to do

    glDeleteBuffers(1, &m_VTX);
    glDeleteBuffers(1, &m_IDX);
    glDeleteBuffers(1, &m_POS);
    glDeleteBuffers(1, &m_ROT);
    glDeleteBuffers(1, &m_SCL);
    glDeleteBuffers(1, &m_CLR);
    glDeleteBuffers(1, &m_TEX);
    
    glDeleteVertexArrays(1, &m_VAO);

    m_MaxNumber = 0;
    m_VTX = m_IDX = m_VAO = 0;
    m_POS = m_ROT = m_SCL = m_CLR = m_TEX = 0;

    m_Position.clear();
    m_Rotation.clear();
    m_Scale.clear();
    m_Color.clear();
    m_Texture.clear();
}

Object::Object(Object&& obj) noexcept {
    std::swap(m_MaxNumber, obj.m_MaxNumber);
    std::swap(m_VAO, obj.m_VAO);
    std::swap(m_VTX, obj.m_VTX);
    std::swap(m_IDX, obj.m_IDX);
    std::swap(m_POS, obj.m_POS);
    std::swap(m_ROT, obj.m_ROT);
    std::swap(m_SCL, obj.m_SCL);
    std::swap(m_CLR, obj.m_CLR);
    std::swap(m_TEX, obj.m_TEX);
    std::swap(m_NumIndices, obj.m_NumIndices);
}

Object& Object::operator=(Object&& obj) noexcept {
    if (&obj != this) {
        this->~Object();
        new (this) Object(std::move(obj));
    }
    return *this;
}

void Object::initialize(const std::vector<Vertex>& vtxBuffer, const std::vector<glm::uvec3>& idxBuffer) {
    ASSERT(m_NumIndices == 0, "Object was already initialized!");
    m_NumIndices = static_cast<GLsizei>(3 * idxBuffer.size());

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);;

    glGenBuffers(1, &m_VTX);
    glBindBuffer(GL_ARRAY_BUFFER, m_VTX);
    glBufferData(GL_ARRAY_BUFFER, vtxBuffer.size() * sizeof(Vertex), vtxBuffer.data(), GL_DYNAMIC_DRAW);

    // layout for buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);
    
    // setup index buffer
    glGenBuffers(1, &m_IDX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IDX);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBuffer.size() * sizeof(glm::uvec3), idxBuffer.data(), GL_STATIC_DRAW);
 
    // INSTANCING ///////////////////////////////////////////////////////////////////////
    
    auto foo = [](uint32_t& bufID, uint32_t id, uint32_t size,  uint32_t maxNumber, size_t bytes) -> void {
        glGenBuffers(1, &bufID);
        glBindBuffer(GL_ARRAY_BUFFER, bufID);
        glBufferData(GL_ARRAY_BUFFER, maxNumber * bytes, nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(id, size, GL_FLOAT, GL_FALSE, bytes, nullptr);
        glEnableVertexAttribArray(id);
        glVertexAttribDivisor(id, 1);
    };

    foo(m_POS, 3, 3, m_MaxNumber, sizeof(glm::vec3));
    foo(m_ROT, 4, 3, m_MaxNumber, sizeof(glm::vec3));
    foo(m_SCL, 5, 3, m_MaxNumber, sizeof(glm::vec3));
    foo(m_CLR, 6, 4, m_MaxNumber, sizeof(glm::vec4));
    foo(m_TEX, 7, 1, m_MaxNumber, sizeof(int32_t));
}

void Object::draw(void) {
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VTX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IDX);
    
    const uint32_t numBodies = static_cast<uint32_t>(m_Position.size());

    glBindBuffer(GL_ARRAY_BUFFER, m_POS);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBodies * sizeof(glm::vec3), m_Position.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, m_ROT);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBodies * sizeof(glm::vec3), m_Rotation.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_SCL);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBodies * sizeof(glm::vec3), m_Scale.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_CLR);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBodies * sizeof(glm::vec4), m_Color.data());

    glBindBuffer(GL_ARRAY_BUFFER, m_TEX);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBodies * sizeof(int32_t), m_Texture.data());


    glDrawElementsInstanced(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr, numBodies);

    // Clearing up for next round
    m_Position.clear();
    m_Rotation.clear();
    m_Scale.clear();
    m_Color.clear();
    m_Texture.clear();
}

void Object::submit(const Specification& specs) {
    m_Position.push_back(specs.position);
    m_Rotation.push_back(specs.rotation);
    m_Scale.push_back(specs.scale);
    m_Color.push_back(specs.color);
    m_Texture.push_back(specs.textureID);
    ASSERT(m_Position.size() <= m_MaxNumber, "More objects submitted than allocated :: " + std::to_string(m_Position.size()) + " > " + std::to_string(m_MaxNumber));
}

} // namespace GRender
