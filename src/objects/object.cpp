

#include "GRender/objects/object.h"

namespace GRender {

constexpr std::string_view vertexShader =
    "#version 450 core                                 \n"
    "layout(location = 0) in vec3 vPosition;           \n"
    "layout(location = 1) in vec3 vNormal;             \n"
    "layout(location = 2) in vec2 vTexCoord;           \n"
    "layout(location = 3) in vec3 bPosition;           \n"
    "layout(location = 4) in vec3 bRotate;             \n"
    "layout(location = 5) in vec3 bScale;              \n"
    "layout(location = 6) in vec4 bColor;              \n"
    "layout(location = 7) in int bTexID;               \n"
    "                                                  \n"
    "uniform mat4 u_transform;                         \n"
    "                                                  \n"
    "out flat int  fTexID;                             \n"
    "out vec2 fTexCoord;                               \n"
    "out vec4 fColor;                                  \n"
    "out vec3 fNormal;                                 \n"
    "out vec3 fPos;                                    \n"
    "                                                  \n"
    "                                                  \n"
    "mat2 Rotate(float angle) {                        \n"
    "    float c = cos(angle), s = sin(angle);         \n"
    "    return mat2(c, s, -s, c);                     \n"
    "}                                                 \n"
    "                                                  \n"
    "void main() {                                     \n"
    "    mat2 rot;                                     \n"
    "                                                  \n"
    "    // Setup color and texture                    \n"
    "    fTexID = bTexID;                              \n"
    "    fTexCoord = vTexCoord;                        \n"
    "    fColor = bColor;                              \n"
    "                                                  \n"
    "    // Working on position and normal             \n"
    "    fNormal = vNormal;                            \n"
    "    fPos = bScale * vPosition;                    \n"
    "                                                  \n"
    "    // Rotate in x                                \n"
    "    rot = Rotate(bRotate.x);                      \n"
    "    fNormal.zy = rot * fNormal.zy;                \n"
    "    fPos.zy = rot * fPos.zy;                      \n"
    "                                                  \n"
    "    // Rotate in y                                \n"
    "    rot = Rotate(bRotate.y);                      \n"
    "    fNormal.xz = rot * fNormal.xz;                \n"
    "    fPos.xz = rot * fPos.xz;                      \n"
    "                                                  \n"
    "    // Rotate in z                                \n"
    "    rot = Rotate(bRotate.z);                      \n"
    "    fNormal.xy = rot * fNormal.xy;                \n"
    "    fPos.xy = rot * fPos.xy;                      \n"
    "                                                  \n"
    "    // Translating position                       \n"
    "    fPos += bPosition;                            \n"
    "                                                  \n"
    "    gl_Position = u_transform * vec4(fPos, 1.0);  \n"
    "}                                                 \n";

constexpr std::string_view fragmentShader =
    "#version 450 core                                                              \n"
    "in flat int fTexID;                                                            \n"
    "in vec2 fTexCoord;                                                             \n"
    "in vec4 fColor;                                                                \n"
    "in vec3 fNormal;                                                               \n"
    "in vec3 fPos;                                                                  \n"
    "                                                                               \n"
    "layout(location = 0) out vec4 fragColor;                                       \n"
    "                                                                               \n"
    "uniform sampler2D texSampler[32];                                              \n"
    "                                                                               \n"
    "void main() {                                                                  \n"
    "    vec3 lightPos = vec3(0.0,100.0,50.0);                                      \n"
    "    vec3 lightDir = normalize(lightPos - fPos);                                \n"
    "                                                                               \n"
    "    float ambientLight = 0.3;                                                  \n"
    "    float diffuse = max(dot(fNormal, lightDir), 0.0);                          \n"
    "                                                                               \n"
    "                                                                               \n"
    "    vec3 color = fColor.rgb;                                                   \n"
    "	if (fTexID >= 0) { color *= texture(texSampler[fTexID], fTexCoord).rgb; }   \n"
    "                                                                               \n"
    "    color *= (ambientLight + diffuse);                                         \n"
    "    fragColor = vec4(color, 1.0);                                              \n"
    "}                                                                              \n";

std::unique_ptr<Shader> Object::m_Shader = nullptr;

/// OBJECT IMPLEMENTATION ///////////////////////////////////////////////////////////////

using Vertex = object::Vertex;
using Specification = object::Specification;

Object::Object(uint32_t maxNumber) : m_MaxNumber(maxNumber) {
    m_Position.reserve(maxNumber);
    m_Rotation.reserve(maxNumber);
    m_Scale.reserve(maxNumber);
    m_Color.reserve(maxNumber);
    m_Texture.reserve(maxNumber);

    // We need to initialize the shader the first time Object is created
    if (m_Shader == nullptr) {
        const fs::path vtxPath = fs::temp_directory_path() / std::to_string(std::random_device()());
        const fs::path fragPath = fs::temp_directory_path() / std::to_string(std::random_device()());

        auto saveFile = [](const fs::path& filePath, const std::string_view& data) { std::ofstream(filePath) << data; };
        saveFile(vtxPath, vertexShader);
        saveFile(fragPath, fragmentShader);

        m_Shader = std::make_unique<Shader>(vtxPath, fragPath);
        fs::remove(vtxPath);
        fs::remove(fragPath);
    }
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
        glVertexAttribPointer(id, size, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(bytes), nullptr);
        glEnableVertexAttribArray(id);
        glVertexAttribDivisor(id, 1);
    };

    foo(m_POS, 3, 3, m_MaxNumber, sizeof(glm::vec3));
    foo(m_ROT, 4, 3, m_MaxNumber, sizeof(glm::vec3));
    foo(m_SCL, 5, 3, m_MaxNumber, sizeof(glm::vec3));
    foo(m_CLR, 6, 4, m_MaxNumber, sizeof(glm::vec4));
    foo(m_TEX, 7, 1, m_MaxNumber, sizeof(int32_t));
}

void Object::draw(const glm::mat4& viewMatrix) {
    // Preparing shader for rendering
    m_Shader->bind();
    m_Shader->setUniform("u_transform", viewMatrix);

    for (auto [tex, id] : m_TextureMap) {
        m_Shader->setTexture(*tex, id);
    }

    // Binding buffer for drawing
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VTX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IDX);
    // Submitting data into graphics card
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

    // Drawing all objects in one call
    glDrawElementsInstanced(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, nullptr, numBodies);

    // Clearing up for next round
    m_Position.clear();
    m_Rotation.clear();
    m_Scale.clear();
    m_Color.clear();
    m_Texture.clear();
    m_TextureMap.clear();
}

void Object::submit(const Specification& specs) {
    int32_t texId = -1;
    if (specs.texture) {
        auto it = m_TextureMap.emplace(specs.texture, static_cast<int32_t>(m_TextureMap.size()));
        texId = it.first->second;
        ASSERT(texId < 32, "Cannot use more than 32 textures at the same draw call");
    }

    m_Position.push_back(specs.position);
    m_Rotation.push_back(specs.rotation);
    m_Scale.push_back(specs.scale);
    m_Color.push_back(specs.color);
    m_Texture.push_back(texId);
    ASSERT(m_Position.size() <= m_MaxNumber, "More objects submitted than allocated :: " + std::to_string(m_Position.size()) + " > " + std::to_string(m_MaxNumber));
}

} // namespace GRender
