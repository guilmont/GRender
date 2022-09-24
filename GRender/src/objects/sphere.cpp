#include <math.h>

#include "GRender/objects/sphere.h"
namespace GRender {

inline size_t startIndex(size_t k) { return 9 + (4*k + 1) * (k -1); }

Sphere::Sphere(const uint32_t maxNumSpheres) : Object(maxNumSpheres) {
    // This implementation of sphere is loosely based on a cube with N-1 subdivisions per face.
    std::vector<object::Vertex> vtxBuffer;

    const size_t N = 13;  // must be >= 3 and odd
    const size_t NY = N + 2 * (N >> 1);
    float dTheta = glm::pi<float>() / float(NY - 1);

    // Creating vertices
    for (size_t k = 1; k < NY-1; k++) {
        size_t NX = k < (N + (N>>1)) ? std::min(2*k+1, N) : 2 * (NY - k - 1) + 1;

        float theta = k * dTheta,
              dPhi = glm::half_pi<float>() / (NX - 1);

        size_t maxL = 4 * (NX-1) + 1;

        // Top-most
        if (k == 1) {
            for (size_t l = 0; l < maxL; l++) {
                object::Vertex v;
                float phi = l * dPhi;
                v.position = {0.0f, 0.5f, 0.0f};
                v.normal   = {0.0f, 1.0f, 0.0f};
                v.texCoord = {1.0f -  phi / glm::two_pi<float>(), 1.0f};
                vtxBuffer.emplace_back(v);
            }
        }

        // Bulk
        for (size_t l = 0; l < maxL; l++) {
            float phi = l * dPhi;
            object::Vertex v;
            v.normal = {cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta)};
            v.position = 0.5f * v.normal;
            v.texCoord = {1.0f - phi / glm::two_pi<float>(), 1.0f - theta / glm::pi<float>() };
            vtxBuffer.emplace_back(v);
        }

        // Bottom most
        if (k == NY - 2) {
            for (size_t l = 0; l < maxL; l++) {
                object::Vertex v;
                float phi = l * dPhi;
                v.position = {0.0f, -0.5f, 0.0f};
                v.normal   = {0.0f, -1.0f, 0.0f};
                v.texCoord = {1.0f - phi / glm::two_pi<float>(), 0.0f};
                vtxBuffer.emplace_back(v);
            }   
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Working on index buffer
    std::vector<glm::uvec3> idxBuffer; 

    // Top cap
    size_t s1 = 0, s2 = 9;
    for(size_t l = 0; l < 8; l++) {
        idxBuffer.emplace_back(s1, s2++, s2);
        idxBuffer.emplace_back(s1++, s2, s1);
    }

    // Top bulk
    for (size_t k = 1; k < (N >> 1); k++) {
       // tracks the corners
       size_t r = k;
           
       // Getting global index to vertices
       size_t s10 = startIndex(k),
              s20 = startIndex(k+1),
              s1 = s10, s2 = s20;

       for (size_t l = 0; l < (8 * k + 1); l++) {
           // corner
           if (l == r) {
               r += 2 * k;
               idxBuffer.emplace_back(s1, s2++, s2);
               idxBuffer.emplace_back(s1, s2++, s2);
           }
           // normal
           idxBuffer.emplace_back(s1, s2++, s2);
           idxBuffer.emplace_back(s1++, s2, s1);
       }
    }

    // Total number of triangles produces until here.
    // We will use it to produces the bottom portion of the sphere
    const size_t num2bulk = idxBuffer.size();

    // Bulk
    for (size_t k = N >> 1; k < N + (N >> 1) - 1; k++) {
       size_t s10 = startIndex(N>>1) + (k - (N >> 1)) * (4 * (N - 1) + 1);
       size_t s20 = s10 + 4 * (N - 1) + 1;
       size_t s1 = s10, s2 = s20;

       for (size_t l = 0; l <= 4 * (N - 1) - 1; l++) {
           idxBuffer.emplace_back(s1, s2++, s2);
           idxBuffer.emplace_back(s1++, s2, s1);
       }
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
