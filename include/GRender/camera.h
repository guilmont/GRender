#pragma once

#include "core.h"

namespace GRender {

class Camera {
public:
    // x: left to right; y: bottom to top; z: positive axis into screen
    // yaw (x): 0..2pi; pitch(bot->top): -pi/2..pi/2
    Camera(const glm::vec3& defPosition = {0.0f, 0.0f, 0.0f},
           float defPitch = 0.0f, float defYaw = -glm::half_pi<float>());
    ~Camera(void) = default;

    // displaying showing camera data and parameters
    virtual void display(void);
    void open(void);
    void close(void);

    // Transformations
    virtual glm::mat4 getViewMatrix(void);
    virtual void reset(void);

    // allows for automatic keyboard and mouse input handling
    virtual void controls(float deltaTime);

    glm::vec3& position(void) { return m_Position; }
    glm::vec2& angles(void) { return m_Angles; }
    float& fOV(void) { return m_FOV; }
    float& speed(void) { return m_Speed; }
    float& sensitivity(void) { return m_Sensitivity; }
    float& aspectRatio(void) { return m_Ratio; }
    // Default values for reset
    glm::vec3& defaultPosition(void) { return m_DefPosition; }
    glm::vec2& defaultAngles(void) { return m_DefAngles; }

    // To use with "const Camera"
    glm::vec3 position(void) const { return m_Position; }
    glm::vec2 angles(void) const { return m_Angles; }
    glm::vec3 defaultPosition(void) const { return m_DefPosition; }
    glm::vec2 defaultAngles(void) const { return m_DefAngles; }
    float fOV(void) const { return m_FOV; }
    float speed(void) const { return m_Speed; }
    float sensitivity(void) const { return m_Sensitivity; }
    float aspectRatio(void) const { return m_Ratio; }

protected:
    void calculateFront();

protected:
    bool m_Active = false;

    float m_FOV = glm::radians(45.0f),
          m_Near = 0.1f,
          m_Far = 1000.0f,
          m_Ratio = 1.0f;

    float m_Speed = 10.0f,
          m_Sensitivity = 0.1f;

    glm::vec2 m_Angles = {-glm::half_pi<float>(), 0.0f},
              m_DefAngles = {-glm::half_pi<float>(), 0.0f};

    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f},
              m_DefPosition = {0.0f, 0.0f, 0.0f},
              m_Front = {-1.0f, 0.0f, 0.0f},
              m_Side = { 0.0f, 0.0f, 1.0f};

private:
    void lookAround(const glm::vec2& offset);

};

} // namespace GRender