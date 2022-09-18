

#include "GRender/objects/object.h"

namespace GRender {

Object::Object(uint32_t maxNumber) : m_MaxNumber(maxNumber) {
    m_VecPosition.reserve(maxNumber);
    m_VecColor.reserve(maxNumber);
}


Object::~Object(void) {
    glDeleteBuffers(1, &m_VTX);
    glDeleteBuffers(1, &m_IDX);
    
    glDeleteBuffers(1, &m_Position);
    glDeleteBuffers(1, &m_Color);

    glDeleteVertexArrays(1, &m_VAO);

    m_MaxNumber = 0;
    m_VTX = m_IDX = m_VAO = 0;
    m_Position = m_Color = 0;
}

Object::Object(Object&& obj) noexcept {
    std::swap(m_MaxNumber, obj.m_MaxNumber);
    std::swap(m_VAO, obj.m_VAO);
    std::swap(m_VTX, obj.m_VTX);
    std::swap(m_IDX, obj.m_IDX);

    std::swap(m_Position, obj.m_Position);
    std::swap(m_Color, obj.m_Color);

    std::swap(m_NumIndices, obj.m_NumIndices);
    m_VecPosition.clear();
    m_VecColor.clear();
}

Object& Object::operator=(Object&& obj) noexcept {
    if (&obj != this) {
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
    // Position
    glGenBuffers(1, &m_Position);
    glBindBuffer(GL_ARRAY_BUFFER, m_Position);
    glBufferData(GL_ARRAY_BUFFER, m_MaxNumber * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // Color
    glGenBuffers(1, &m_Color);
    glBindBuffer(GL_ARRAY_BUFFER, m_Color);
    glBufferData(GL_ARRAY_BUFFER, m_MaxNumber * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);
}

void Object::draw(void) {
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VTX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IDX);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_Position);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_VecPosition.size() * sizeof(glm::vec3), m_VecPosition.data());
    
    glBindBuffer(GL_ARRAY_BUFFER, m_Color);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_VecColor.size() * sizeof(glm::vec3), m_VecColor.data());

    glDrawElementsInstanced(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr, static_cast<int32_t>(m_VecPosition.size()));

    // Clearing up for next round
    m_VecPosition.clear();
    m_VecColor.clear();
}

void Object::submit(const glm::vec3& position, const glm::vec3& color) {
    m_VecPosition.push_back(position);
    m_VecColor.push_back(color);
    ASSERT(m_VecPosition.size() <= m_MaxNumber, "More cubes submitted than allocated!");
}

} // namespace GRender
