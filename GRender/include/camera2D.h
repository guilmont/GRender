#pragma once

#include "core.h"

namespace GRender
{
    class Camera2D
    {
    private:
        float
            fNear = 0.001f,
            fFar = 1000.0f,
            maxZoom = 0.05f,
            minZoom = 1.0f,
            speed = 1.0f,
            sensitivity = 0.2f,
            aspect = 1.0f;
  
        glm::vec3 position = { 0.0f, 0.0f, 1.0f };

    public:
        Camera2D(void) = default;
        ~Camera2D(void) = default;

        glm::vec3& getPosition(void) { return position; }
        float& getSensitivity(void) { return sensitivity; }

        glm::mat4 getViewMatrix(void);
        float getZoom(void) const { return position.z; }

        void setAspectRatio(float value) { aspect = value; }

        void moveFront(float elapsed);
        void moveBack(float elapsed);
        void moveUp(float elapsed);
        void moveDown(float elapsed);
        void moveLeft(float elapsed);
        void moveRight(float elapsed);
        
        void moveHorizontal(float value);
        void moveVertical(float value);
    };

}
