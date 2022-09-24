#include <random>
#include <vector>
#include <algorithm>

#include "polymer.h"

namespace polymer {

Polymer::Polymer(uint32_t numBeads, float kuhn) : m_NumBeads(numBeads), m_Kuhn(kuhn), m_Radius(0.5f*kuhn) {
    m_Sphere = GRender::Sphere(numBeads);
    m_Cylinder = GRender::Cylinder(numBeads-1);    

    // Creating random location for all beads
    std::default_random_engine ran(123456);
    std::normal_distribution<float> normal(0.0f, kuhn / sqrt(3.0f));

    // Calculating position for spheres
    glm::vec3 com = { 0.0f, 0.0f, 0.0f }; // center of mass -> will be set to zero later
 
    m_Position.resize(m_NumBeads);
    m_Position.at(0) = { 0.0f, 0.0f, 0.0f };
    for (uint64_t k = 1; k < numBeads; k++)
    {
        m_Position.at(k).x = m_Position.at(k - 1).x + normal(ran);
        m_Position.at(k).y = m_Position.at(k - 1).y + normal(ran);
        m_Position.at(k).z = m_Position.at(k - 1).z + normal(ran);
        com += m_Position.at(k);
    }

    // Bringing center of mass to zero
    com /= float(numBeads);
    std::for_each(m_Position.begin(), m_Position.end(), [&com](glm::vec3& p) { p -= com; });

    // Calculating position, length and orientation of tubes
    for (size_t k = 0; k < numBeads - 1; k++) {
        CData dt;
        dt.position = 0.5f * (m_Position[k] + m_Position[k+1]);
        dt.angle = GRender::Cylinder::calcAnglesFromDirection(m_Position[k+1] - m_Position[k]);
        dt.height = glm::distance(m_Position[k], m_Position[k+1]);

        m_Tubes.emplace_back(dt);
    }
}


Polymer::Polymer(Polymer&& poly) noexcept {
    std::swap(m_NumBeads, poly.m_NumBeads);
    std::swap(m_Kuhn, poly.m_Kuhn);
    std::swap(m_SphereColor, poly.m_SphereColor);
    std::swap(m_CylinderColor, poly.m_CylinderColor);
    
    std::swap(m_Position, poly.m_Position);
    std::swap(m_Tubes, poly.m_Tubes);
    std::swap(m_Sphere, poly.m_Sphere);
    std::swap(m_Cylinder, poly.m_Cylinder);
}

Polymer& Polymer::operator=(Polymer&& poly) noexcept {
    if (&poly != this) {
        this->~Polymer();
        new(this) Polymer(std::move(poly));
    }
    return *this;
}


void Polymer::draw(void) {
    GRender::object::Specification obj;
    obj.color = m_SphereColor;
    obj.scale = glm::vec3(2.0f * m_Radius);
    for (const glm::vec3& pos : m_Position) {
        obj.position = pos;
        m_Sphere.submit(obj);
    }
    m_Sphere.draw();

    obj.color = m_CylinderColor;
    obj.scale = {  1.0f, m_Radius, m_Radius };
    for (const CData& dt : m_Tubes) {
        obj.position = dt.position;
        obj.rotation = dt.angle;
        obj.scale.x = dt.height;
        m_Cylinder.submit(obj);
    }
    m_Cylinder.draw();
}


} // namespace polymer