#pragma once

#include "core.h"

namespace GRender {

class Camera {
public:
    // x: left to right; y: bottom to top; z: positive axis into screen
    // yaw (x): 0..2pi; pitch(bot->top): -pi/2..pi/2
    Camera(const glm::vec3& defPosition = {0.0f, 0.0f, 0.0f}, float defPitch = 0.0f, float defYaw = 1.5708f);
    ~Camera(void) = default;

    // displaying showing camera data and parameters
    void display(void);
    void open(void);
    void close(void);
    
    // Transformations
    glm::mat4 getViewMatrix(void);
    void reset(void);

    // allows for automatic keyboard and mouse input handling
    void controls(float deltaTime);

    // Movement
    void lookAround(const glm::vec2& offset, float elapsed);
    void moveFront(float elapsed);
    void moveBack(float elapsed);
    void moveUp(float elapsed);
    void moveDown(float elapsed);
    void moveLeft(float elapsed);
    void moveRight(float elapsed);


    // Getters/setters
    float getYaw(void) const;
    void setYaw(float value);

    float getPitch(void) const;
    void setPitch(float value);

    float getFOV(void) const;
    void setFOV(float fov);

    float getAspectRatio(void) const;
    void setAspectRatio(float aRatio);

    const glm::vec3& getPosition(void) const;
    void setPosition(const glm::vec3& pos);

    // Default values for reset
    const glm::vec3& getDefaultPosition(void) const;
    void setDefaultPosition(const glm::vec3& pos);
   
    float getDefaultPitch(void) const;
    void setDefaultPitch(float pitch);
   
    float getDefaultYaw(void) const;
    void setDefaultYaw(float yaw);

private:
    bool active = false;
    void calculateFront();

private:
    float
        mFOV = glm::radians(45.0f),
        mNear = 0.1f,
        mFar = 1000.0f,
        mRatio = 1.0f;
        
    float
        mSpeed = 10.0f,
        mSensitivity = 0.1f,
        mPitch, mDefPitch,
        mYaw, mDefYaw;

    glm::vec3
        mDefPosition, mPosition, mFront;
};

} // namespace GRender