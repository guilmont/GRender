#pragma once

#include "core.h"

namespace GRender {

class Camera {
public:
    // x -> left to right
    // y -> bottom to top
    // z -> positive axis into screen
    Camera(const glm::vec3& defaultPosition = {0.0f, 0.0f, 0.0f}, const glm::vec3& defaultFront = {0.0f, 0.0f, 1.0f});
    ~Camera(void) = default;

    void open(void);
    void close(void);
    void showControls(void);

    // Transformations
    glm::mat4 getViewMatrix(void);
    void reset(void);

    // Movement
    void lookAround(const glm::vec2& offset, float elapsed);
    void moveFront(float elapsed);
    void moveBack(float elapsed);
    void moveUp(float elapsed);
    void moveDown(float elapsed);
    void moveLeft(float elapsed);
    void moveRight(float elapsed);


    // Getters/setters
    float& aspectRatio(void);

    float& yaw(void);
    float& pitch(void);
    glm::vec3& position(void);

    glm::vec3 front(void) const; // controlled by yaw and pitch
    
    void setDefaultPosition(const glm::vec3& pos);
    void setDefaultFront(const glm::vec3& front);

private:
    bool active = false;

private:
    float
        mFOV = glm::radians(45.0f),
        mNear = 0.1f,
        mFar = 1000.0f,
        mRatio = 1.0f;
        
    float
        mSpeed = 10.0f,
        mSensitivity = 0.1f,
        mYaw,
        mPitch;

    glm::vec3
        mDefaultPosition, mPosition,
        mDefaultFront, mFront;
};

} // namespace GRender