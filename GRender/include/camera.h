#pragma once

#include "core.h"

namespace GRender
{

    class Camera
    {
    public:
        Camera(void) = default;
         ~Camera(void) = default;

        glm::mat4 getViewMatrix(void);
        void reset(void);

        
        void setAspectRatio(float value) { aspect = value; }

        void moveFront(float elapsed);
        void moveBack(float elapsed);

        void moveUp(float elapsed);
        void moveDown(float elapsed);
        void moveLeft(float elapsed);
        void moveRight(float elapsed);

        void lookAround(const glm::vec2& offset, float elapsed);

        bool viewControls = true;
        void controls(void);

        const glm::vec3& getFront() { return front; }
        const glm::vec3& getPosition() { return position; }
        float& getYaw() { return yaw; }
        float& getPitch() { return pitch;  }


    private:
        float
            fov = glm::radians(45.0f),
            fNear = 0.1f,
            fFar = 1000.0f,
            aspect = 1.0f;
        
        float
            speed = 10.0f,
            sensitivity = 0.333f,
            yaw = -1.570796f,
            pitch = 0.0f;

        glm::vec3
            front = { 0.0f, 0.0f, -1.0f },
            up = { 0.0f, 1.0f, 0.0f },
            position = { 0.0f, 0.0f, 3.0f };

    };

}