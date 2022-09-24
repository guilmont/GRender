#include <math.h>

#include "GRender/objects/cylinder.h"

namespace GRender {

Cylinder::Cylinder(const uint32_t maxNumCylinders) : Object(maxNumCylinders) {

    const size_t N = 30;
    const float dPhi = glm::two_pi<float>() / float(N-1);
    
    std::vector<glm::vec3> vecData;
    for (size_t k = 0; k < N; k++) {
        float phi  = k * dPhi;
        vecData.emplace_back(cos(phi), sin(phi), phi / glm::two_pi<float>());
    }

    // Creating vertices
    float h = 1.0f;
    object::Vertex v;
    std::vector<object::Vertex> vtxBuffer;

    // Top
    for (float r : {0.0f, 0.5f}) {
        for (const glm::vec3& val : vecData) {
            v.position = { 0.5f, r * val.x, r * val.y }; 
            v.normal =   { 1.0f, 0.0f, 0.0f };
            v.texCoord = { val.z, h};
            vtxBuffer.push_back(v);
        }
        h = 0.95f;
    }

    // Sides
    for (float y : {1.0f, 0.0f}) {
        for (const glm::vec3& val : vecData) {
            v.position = {  -0.5 + y, 0.5f * val.x, 0.5f * val.y };
            v.normal = { 0.0f, val.x, val.y };
            v.texCoord = { val.z, h };
            vtxBuffer.push_back(v);
        }
        h = 0.05f;
    }

    // Bottom
    for (float r : {0.5f, 0.0f}) {
        for (const glm::vec3& val : vecData) {
            v.position = { -0.5f, r * val.x, r * val.y };
            v.normal = { -1.0f, 0.0f, 0.0f };
            v.texCoord = { val.z, h };
            vtxBuffer.push_back(v);
        }
        h = 0.0f;
    }

    // Calculating indices
    std::vector<glm::uvec3> idxBuffer;

    size_t s1 = 0, s2 = N;
    for (size_t k = 0; k < 5; k++) {
       for (size_t l = 0; l < N; l++) {
           idxBuffer.emplace_back(s1, s2++, s2);
           idxBuffer.emplace_back(s1++, s2, s1);
       }
    }

    // Sending all this data into the GPU
    initialize(vtxBuffer, idxBuffer);
}

Cylinder::Cylinder(Cylinder&& obj) noexcept : Object(std::move(obj)) {}

Cylinder& Cylinder::operator=(Cylinder&& obj) noexcept {
    if (&obj != this) {
        new (this) Cylinder(std::move(obj));
    }
    return *this;
}


} // namespace GRender
