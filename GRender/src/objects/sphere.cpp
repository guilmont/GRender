#include <math.h>

#include "GRender/objects/sphere.h"
namespace GRender {

constexpr float PI = 3.14159265358979323846f;

static std::ostream& operator<<(std::ostream& out, const glm::uvec3& v) {
    out << v.x << ", " << v.y << ", " << v.z;
    return out;
}

Sphere::Sphere(const uint32_t maxNumSpheres) : Object(maxNumSpheres) {
    // This implementation of sphere is loosely based on a cube with N-1 subdivisions per face.
    std::vector<object::Vertex> vtxBuffer;

    const size_t N = 11;  // must be >= 3 and odd
    const size_t NY = N + 2 * (N >> 1);
    float dTheta = PI / (NY - 1);

    // Top most vertex
    object::Vertex v;
    v.normal = {0.0f, 1.0f, 0.0f};
    v.position = {0.0f, 0.5f, 0.0f};
    v.texCoord = {1.0f, 1.0f};
    vtxBuffer.push_back(v);

    // Core vertices
    for (size_t k = 0; k < NY; k++) {
        size_t NX = k < (N + (N>>1)) ? std::min(2*k+1, N) : 2 * (NY - k - 1) + 1;

        float theta = k * dTheta,
              dPhi = 0.5f * PI / (NX - 1);

        for (size_t l = 0; l < 4*(NX-1); l++) {
            float phi = l * dPhi;

            v.normal = {cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta)};
            v.position = 0.5f * v.normal;
            v.texCoord = {1.0f - 0.5f * phi / PI, 1.0f - theta / PI};
            vtxBuffer.push_back(v);
        }
    }

    // Bottom most vertex
    v.normal = {0.0f, -1.0f, 0.0f};
    v.position = {0.0f, -0.5f, 0.0f};
    v.texCoord = {0.0f, 0.0f};
    vtxBuffer.emplace_back(v);

    /////////////////////////////////////////////////////////////////////////////////.......
    // Working on index buffer

    // Top cap
    std::vector<glm::uvec3> idxBuffer = { {0,1,2}, {0,2,3}, {0,3,4}, {0,4,5},
                                          {0,5,6}, {0,6,7}, {0,7,8}, {0,8,1} };
    // Top bulk
    for (size_t k = 1; k < (N >> 1); k++) {
        // tracks the corners
        size_t r = k;
            
        // Getting global index to vertices
        size_t s10 = 1 + 4 * k * (k - 1),
               s20 = 1 + 4 * k * (k + 1),
               s1 = s10, s2 = s20;

        for (size_t l = 0; l < 8 * k; l++) {
            // corner
            if (l == r) {
                r += 2 * k;
                idxBuffer.emplace_back(s1, s2++, s2);
                idxBuffer.emplace_back(s1, s2++, s2);
            }
            // normal
            if (s1 + 1 == s20) { break; }
            idxBuffer.emplace_back(s1, s2++, s2);
            idxBuffer.emplace_back(s1++, s2, s1);
        }

        // looping back to start
        idxBuffer.emplace_back(s1, s2, s20);
        idxBuffer.emplace_back(s1, s20, s10);
    }

    // Total number of triangles produces until here. We will use it to produces the bottom portion of the sphere
    const size_t num2bulk = idxBuffer.size();

    // Bulk
    for (size_t k = N >> 1; k < N + (N >> 1) - 1; k++) {
        size_t s10 = 1 + 4 * (N >> 1) * ((N >> 1) - 1) + (k - (N >> 1)) * 4 * (N - 1);
        size_t s20 = s10 + 4 * (N - 1);
        size_t s1 = s10, s2 = s20;

        for (size_t l = 0; l < 4 * (N - 1) - 1; l++) {
            idxBuffer.emplace_back(s1, s2++, s2);
            idxBuffer.emplace_back(s1++, s2, s1);
        }
        
        // looping back to start
        idxBuffer.emplace_back(s1, s2, s20);
        idxBuffer.emplace_back(s1, s20, s10);
    }

    // The bottom portion is simply the reverse of top
    const uint32_t totVtx = static_cast<uint32_t>(vtxBuffer.size()-1);

    for (int64_t k = num2bulk-1; k >= 0; k--) {
        idxBuffer.emplace_back(glm::uvec3{ totVtx } - idxBuffer[k]);
    }

    // Sending all this data into the GPU
    initialize(vtxBuffer, idxBuffer);
}

Sphere::Sphere(Sphere&& obj) noexcept : Object(std::move(obj)) {}

Sphere& Sphere::operator=(Sphere&& obj) noexcept {
    if (&obj != this) {
        new (this) Sphere(std::move(obj));
    }
    return *this;
}


} // namespace GRender
