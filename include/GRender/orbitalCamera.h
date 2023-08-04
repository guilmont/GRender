#pragma once

#include "camera.h"

namespace GRender {

class OrbitalCamera : public Camera{
public:
    // x: left to right; y: bottom to top; z: positive axis into screen
    // yaw (x): 0..2pi; pitch(bot->top): -pi/2..pi/2
    OrbitalCamera(const glm::vec3& center = {0.0f, 0.0f, 0.0f},float distance = 1.0f,
                  float theta = 0.0f, float phi = glm::half_pi<float>());
    ~OrbitalCamera(void) = default;

    // displaying showing camera data and parameters
    void display(void) override;
    
    // Transformations
    glm::mat4 getViewMatrix(void) override;
    void reset(void) override;

    // allows for automatic keyboard and mouse input handling
    void controls(float deltaTime) override;

    float& distance(void) { return m_Distance; }
    float& defaultDistance(void) { return m_DefDistance; }

private:
    void orbitOrigin(const glm::vec2& offset);

private:
    float m_Distance = 1.0f,
          m_DefDistance = 1.0f;
};

} // namespace GRender