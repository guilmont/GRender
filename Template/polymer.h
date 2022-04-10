#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glad/glad.h"

namespace polymer {

struct Vertex {
    Vertex(void) = default;
    Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& cor); 

    glm::vec3 pos, normal, color;
};

struct Object3D {
    uint32_t vao, vtx, idx, com, dir;
    std::vector<Vertex> vtxBuffer;
    std::vector<glm::uvec3> idxBuffer;
    glm::vec3 color;
};

class Polymer {
public:
    Polymer(uint32_t numBeads, float kuhn);
    Polymer(void) = default;
    ~Polymer(void);

    // We have some GPU data we don't want to copy
    Polymer(const Polymer&) = delete;
    Polymer& operator=(const Polymer&) = delete;
    // We might want to move polymers around
    Polymer(Polymer&&) noexcept;
    Polymer& operator=(Polymer&&) noexcept;

    uint32_t getNumBeads(void) const;
    float getKuhn(void) const;
    float& getRadius(void);

    glm::vec3 getSphereColor(void) const;
    glm::vec3 getCylinderColor(void) const;
    
    void submitBlobs(void);
    void submitConnections(void);

    void updateSphereColor(const glm::vec3&);
    void updateCylinderColor(const glm::vec3&);

private:
    void createSphere(const glm::vec3& color, uint32_t divisions);
    void createCylinder(const glm::vec3& color, uint32_t divisions);

private:
    uint32_t numBeads = 1;   // number of beads in polymer
    float kuhn = 1.0f;       // defines polymer structure
    float radius = 0.5f;     // drawing radius
    
    Object3D sphere, cylinder;
};

} // namespace polymer