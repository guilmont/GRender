#pragma once

#include "GRender.h"


class Polymer
{

public:
    Polymer(float kuhn, uint64_t numBeads, uint64_t divisions = 20);
    ~Polymer(void);

    void update(const std::vector<glm::vec3> &positions);
    void submit(void);

    // Let's leave them public for now
    float
        sphereRadius = 1.0f,
        cylinderRadius = 0.5f;

    glm::vec3 
        sphereColor = {0.2f, 0.5f, 0.8f},
        cylinderColor = {0.9f, 0.9f, 0.9f};

private:
    uint64_t
        numBeads, // Better to perform local checks 
        divisions,
        maxVertices,
        maxIndices;

private:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;
    };

    uint32_t
        vao, // Vertex array object
        vertex_buffer,
        index_buffer;

    // Small templates with unitary objects
    struct {
        std::vector<Vertex> vtx;
        std::vector<uint32_t> id;
    } sphere, cylinder;

}; // class-object
