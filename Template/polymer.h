#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "glad/glad.h"


class Polymer
{

public:
    Polymer(uint32_t numBeads, uint32_t divisions = 20);
    ~Polymer(void);

    void update(const std::vector<glm::vec3> &positions);
    void submitBlobs(void);
    void submitConnections(void);

    // Let's leave them public for now
    uint32_t numBeads;

    float kuhn = 1.0f;
        
    glm::vec3 
        sphereColor = {0.2f, 0.5f, 0.8f},
        cylinderColor = {0.9f, 0.9f, 0.9f};

    void updateSphereColor(void);
    void updateCylinderColor(void);

private:
    
    struct Vertex
    {
        Vertex(void) = default;
        ~Vertex(void) = default;
        
        Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& cor) 
            : pos(pos), normal(normal), color(cor) {}

        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;
    };

    struct Object3D {
        uint32_t vao, vtx, idx, com, dir;
        std::vector<Vertex> vtxBuffer;
        std::vector<glm::uvec3> idxBuffer; 
    };

    void createSphere(uint32_t divisions);
    void createCylinder(uint32_t divisions);

private:
    Object3D sphere, cylinder;

}; // class-object
