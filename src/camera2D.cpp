#include "camera2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GRender
{

    glm::mat4 Camera2D::getViewMatrix(void)
    {
        glm::vec3 front(0.0, 0.0, -1.0), up(0.0, 1.0, 0.0);
        glm::mat4 proj = glm::perspective(1.5707963f, aspect, fNear, fFar);
        glm::mat4 view = glm::lookAt(position, position + front, up);

        return proj * view;
    } // getViewMatrix

    void Camera2D::moveFront(float elapsed)
    {
        position.z -= speed * elapsed;
        position.z = position.z < maxZoom ? maxZoom : position.z;
    }

    void Camera2D::moveBack(float elapsed)
    {
        position.z += speed * elapsed;
        position.z = position.z > minZoom ? minZoom : position.z;
    }

    void Camera2D::moveUp(float elapsed)    { position.y += speed * position.z * elapsed; }
    void Camera2D::moveDown(float elapsed)  { position.y -= speed * position.z * elapsed; }
    void Camera2D::moveLeft(float elapsed)  { position.x -= speed * position.z * elapsed; }
    void Camera2D::moveRight(float elapsed) { position.x += speed * position.z * elapsed; }

    void Camera2D::moveHorizontal(float value) { position.x -= sensitivity * position.z * value; } // slower the closer to image
    void Camera2D::moveVertical(float value)   { position.y -= sensitivity * position.z * value; } // faster the further from image

}