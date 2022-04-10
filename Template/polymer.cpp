#include <random>
#include <vector>

#include "polymer.h"

namespace polymer {

constexpr float PI = 3.14159265358979323846f;

Vertex::Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec3& cor)
    : pos(pos), normal(normal), color(cor) {}


Polymer::Polymer(uint32_t numBeads, float kuhn) : numBeads(numBeads), kuhn(kuhn), radius(0.5f*kuhn) {
    // Create meshes
    createSphere({ 0.2f, 0.5f, 0.8f }, 50);
    createCylinder({ 0.9f, 0.9f, 0.9f }, 50);

    // Creating random location for all beads
    std::default_random_engine ran(123456);
    std::normal_distribution<float> normal(0.0f, kuhn / sqrt(3.0f));

    glm::vec3 com = { 0.0f, 0.0f, 0.0f }; // center of mass -> will be set to zero later
    std::vector<glm::vec3> vpos(numBeads);

    vpos.at(0) = { 0.0f, 0.0f, 0.0f };
    for (uint64_t k = 1; k < numBeads; k++)
    {
        vpos.at(k).x = vpos.at(k - 1).x + normal(ran);
        vpos.at(k).y = vpos.at(k - 1).y + normal(ran);
        vpos.at(k).z = vpos.at(k - 1).z + normal(ran);
        com += vpos.at(k);
    }

    com /= float(numBeads);
    for (glm::vec3& p : vpos)
        p -= com;

    // Updating sphere positions
    glBindVertexArray(sphere.vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.com);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numBeads * sizeof(glm::vec3), vpos.data());

    // Updating connections
    std::vector<glm::vec3> vecCom(numBeads - 1), vecDir(numBeads - 1);
    for (uint32_t k = 0; k < numBeads - 1; k++)
    {
        vecCom.at(k) = 0.5f * (vpos.at(k) + vpos.at(k + 1));
        vecDir.at(k) = vpos.at(k + 1) - vpos.at(k);
    }

    glBindVertexArray(cylinder.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.com);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vecCom.size() * sizeof(glm::vec3), vecCom.data());

    glBindBuffer(GL_ARRAY_BUFFER, cylinder.dir);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vecDir.size() * sizeof(glm::vec3), vecDir.data());
}

Polymer::~Polymer(void)
{
    glDeleteBuffers(1, &sphere.vtx);
    glDeleteBuffers(1, &sphere.idx);
    glDeleteBuffers(1, &sphere.com);
    glDeleteVertexArrays(1, &sphere.vao);

    glDeleteBuffers(1, &cylinder.vtx);
    glDeleteBuffers(1, &cylinder.idx);
    glDeleteBuffers(1, &cylinder.com);
    glDeleteBuffers(1, &cylinder.dir);
    glDeleteVertexArrays(1, &cylinder.vao);
}

Polymer::Polymer(Polymer&& poly) noexcept {
    std::swap(numBeads, poly.numBeads);
    std::swap(kuhn, poly.kuhn);
    std::swap(sphere, poly.sphere);
    std::swap(cylinder, poly.cylinder);
}

Polymer& Polymer::operator=(Polymer&& poly) noexcept {
    if (&poly != this) {
        new(this) Polymer(std::move(poly));
    }
    return *this;
}

uint32_t Polymer::getNumBeads() const {
    return numBeads;
}

float Polymer::getKuhn() const {
    return kuhn;
}

float& Polymer::getRadius() {
    return radius;
}

glm::vec3 Polymer::getSphereColor() const {
    return sphere.color;
}

glm::vec3 Polymer::getCylinderColor() const {
    return cylinder.color;
}

void Polymer::createSphere(const glm::vec3& color, uint32_t divisions) {
    float dtheta = PI / float(divisions);
    float dphi = 2.0f * PI / float(divisions);

    sphere.color = color;
    sphere.vtxBuffer.emplace_back(glm::vec3{ 0.0, 0.0f, 1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, sphere.color); // top vertex

    for (uint64_t ctt = 1; ctt < divisions - 1; ctt++)
        for (uint64_t ctp = 0; ctp < divisions; ctp++)
        {
            float
                phi = float(ctp) * dphi,
                theta = float(ctt) * dtheta;

            float
                x = sin(theta) * cos(phi),
                y = sin(theta) * sin(phi),
                z = cos(theta);

            sphere.vtxBuffer.emplace_back(glm::vec3{ x, y, z }, glm::vec3{ x, y, z }, sphere.color);
        }

    sphere.vtxBuffer.emplace_back(glm::vec3{ 0.0, 0.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, sphere.color); // bottom vertex

    /////////////////////////////////////////////////////////////////////////////////////

    // Creating indices matrix 
    std::vector<uint32_t> vec(divisions * divisions);
    for (uint32_t k = 0; k < divisions; k++)
        for (uint32_t l = 0; l < divisions; l++) {
            if (k == 0)
                vec[k * divisions + l] = 0;
            else if (k + 1 == divisions)
                vec[k * divisions + l] = uint32_t(sphere.vtxBuffer.size() - 1);
            else
                vec[k * divisions + l] = (k - 1) * divisions + l + 1;
        }

    // Creating index buffer
    for (uint32_t k = 0; k < divisions - 1; k++)
        for (uint32_t l = 0; l < divisions; l++) {
            uint32_t r1, r2, r3;

            if (k > 0) {
                // top triangle
                r1 = k * divisions + l;
                r2 = r1 + divisions;
                r3 = k * divisions + ((l + 1) % divisions);
                sphere.idxBuffer.emplace_back(vec[r1], vec[r2], vec[r3]);
            }

            // bottom triangle
            r1 = (k + 1) * divisions + l;
            r2 = (k + 1) * divisions + ((l + 1) % divisions);
            r3 = k * divisions + ((l + 1) % divisions);
            sphere.idxBuffer.emplace_back(vec[r1], vec[r2], vec[r3]);
        }


    /////////////////////////////////////////////////////////////////////////////////////

    glGenVertexArrays(1, &sphere.vao);
    glBindVertexArray(sphere.vao);

    glGenBuffers(1, &sphere.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.vtx);
    glBufferData(GL_ARRAY_BUFFER, sphere.vtxBuffer.size() * sizeof(Vertex), sphere.vtxBuffer.data(), GL_DYNAMIC_DRAW);

    // layout for buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // instance buffer
    glGenBuffers(1, &sphere.com);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.com);
    glBufferData(GL_ARRAY_BUFFER, numBeads * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // setup index buffer
    glGenBuffers(1, &sphere.idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.idxBuffer.size() * sizeof(glm::uvec3), sphere.idxBuffer.data(), GL_STATIC_DRAW);
}

void Polymer::createCylinder(const glm::vec3& color, uint32_t divisions) {
    cylinder.color = color;

    // We are never going to see the ends of this cylinder, let's save from triangles from there
    float  dphi = 2.0f * PI / float(divisions);

    for (float x = -0.5f; x < 0.51f; x += 1.0f)
        for (uint32_t ct = 0; ct < divisions; ct++) {
            float phi = float(ct) * dphi;

            Vertex v;
            v.pos = { x, cos(phi), sin(phi) };
            v.normal = glm::normalize(glm::vec3{ 0.0, v.pos.y, v.pos.z });
            v.color = cylinder.color;

            cylinder.vtxBuffer.emplace_back(std::move(v));
        }


    // Setting up index buffer
    for (uint32_t k = 0; k < divisions; k++) {
        uint32_t l = (k + 1) % divisions;

        // top triangle
        cylinder.idxBuffer.emplace_back(k, divisions + k, l);

        // bottom triangle
        cylinder.idxBuffer.emplace_back(divisions + k, l, divisions + l);
    }

    /////////////////////////////////////////////////////////////////////////////////////

    glGenVertexArrays(1, &cylinder.vao);
    glBindVertexArray(cylinder.vao);

    // Copying buffer to gpu
    glGenBuffers(1, &cylinder.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.vtx);
    glBufferData(GL_ARRAY_BUFFER, cylinder.vtxBuffer.size() * sizeof(Vertex), cylinder.vtxBuffer.data(), GL_DYNAMIC_DRAW);

    // layout for buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // instance center of mass buffer
    glGenBuffers(1, &cylinder.com);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.com);
    glBufferData(GL_ARRAY_BUFFER, (numBeads - 1) * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // instance direction buffer
    glGenBuffers(1, &cylinder.dir);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.dir);
    glBufferData(GL_ARRAY_BUFFER, (numBeads - 1) * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);


    // setup index buffer
    glGenBuffers(1, &cylinder.idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder.idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder.idxBuffer.size() * sizeof(glm::uvec3), cylinder.idxBuffer.data(), GL_STATIC_DRAW);
}

void Polymer::submitBlobs(void) {
    glBindVertexArray(sphere.vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.com);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.idx);
    glDrawElementsInstanced(GL_TRIANGLES, 3 * GLsizei(sphere.idxBuffer.size()), GL_UNSIGNED_INT, nullptr, int32_t(numBeads));
}

void Polymer::submitConnections(void) {
    glBindVertexArray(cylinder.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.vtx);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.com);
    glBindBuffer(GL_ARRAY_BUFFER, cylinder.dir);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder.idx);
    glDrawElementsInstanced(GL_TRIANGLES, 3 * GLsizei(cylinder.idxBuffer.size()), GL_UNSIGNED_INT, nullptr, int32_t(numBeads - 1));
}

static void update(Object3D& obj, const glm::vec3& color) {
    obj.color = color;
    for (Vertex& v : obj.vtxBuffer)
        v.color = color;

    glBindVertexArray(obj.vao);
    glBindBuffer(GL_ARRAY_BUFFER, obj.vtx);
    glBufferSubData(GL_ARRAY_BUFFER, 0, obj.vtxBuffer.size() * sizeof(Vertex), obj.vtxBuffer.data());
}

void Polymer::updateSphereColor(const glm::vec3& color) {
    update(sphere, color);
}

void Polymer::updateCylinderColor(const glm::vec3& color) {
    update(cylinder, color);
}

} // namespace polymer