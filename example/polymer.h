#pragma once

#include <vector>

#include "GRender/objects/sphere.h"
#include "GRender/objects/cylinder.h"

namespace polymer {

class Polymer {
public:
    Polymer(uint32_t numBeads, float kuhn);
    Polymer(void) = default;
    ~Polymer(void) = default;

    // We have some GPU data we don't want to copy
    Polymer(const Polymer&) = delete;
    Polymer& operator=(const Polymer&) = delete;
    // We might want to move polymers around
    Polymer(Polymer&&) noexcept;
    Polymer& operator=(Polymer&&) noexcept;

    uint32_t numBeads(void) const { return m_NumBeads; }
    float kuhn(void) const { return m_Kuhn; }
    float& radius(void) { return m_Radius; }

    glm::vec3& sphereColor(void) { return m_SphereColor; }
    glm::vec3& cylinderColor(void) { return m_CylinderColor; }

    void draw(const glm::mat4& viewMatrix);

private:
    struct CData {
        glm::vec3 position, angle;
        float height;
    };

private:
    uint32_t m_NumBeads = 1;   // number of beads in polymer
    float m_Kuhn = 1.0f;       // defines polymer structure
    float m_Radius = 0.5f;     // drawing radius
    
    glm::vec3 m_SphereColor = { 0.2f, 0.5f, 0.8f },
              m_CylinderColor = { 0.9f, 0.9f, 0.9f };

    std::vector<glm::vec3> m_Position;
    std::vector<CData> m_Tubes;

    GRender::Sphere m_Sphere;
    GRender::Cylinder m_Cylinder;
};

} // namespace polymer