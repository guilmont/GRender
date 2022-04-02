#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GRender
{
    glm::mat4 Camera::getViewMatrix(void)
    {
        glm::mat4 proj = glm::perspective(fov, aspect, fNear, fFar);
        glm::mat4 view = glm::lookAt(position, position + front, up);
        return proj * view;
    }

    void Camera::reset(void)
    {
        front = { 0.0f, 0.0f, -1.0f };
        position = { 0.0f, 0.0f, 10.0f };
        yaw = -1.570796f;
        pitch = 0.0f;
    }

    void Camera::moveFront(float elapsed) { position += front * speed * elapsed; }
    void Camera::moveBack(float elapsed)  { position -= front * speed * elapsed; }

    void Camera::moveUp(float elapsed)    { position += up * speed * elapsed; }
    void Camera::moveDown(float elapsed)  { position -= up * speed * elapsed; }
    
    void Camera::moveLeft(float elapsed)  { position -= speed * elapsed * glm::normalize(glm::cross(front, up)); }
    void Camera::moveRight(float elapsed) { position += speed * elapsed * glm::normalize(glm::cross(front, up)); }

    void Camera::lookAround(const glm::vec2& offset, float elapsed)
    {
        yaw -= offset.x * sensitivity * elapsed;
        pitch += offset.y * sensitivity * elapsed;

        pitch = std::max(std::min(pitch, 1.0f), -1.0f);
        front = glm::normalize(glm::vec3{cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch)});
    }

    void Camera::controls(void)
    {
        ImGui::Begin("Camera info", &viewControls);

		ImGui::Text("Position: %.2f x %.2f x %.2f", position.x, position.y, position.z);
		ImGui::Text("Front: %.2f : %.2f : %.2f", front.x, front.y, front.z);
		
        ImGui::Spacing();

        ImGui::Text("Pitch: %.2f", pitch);
		ImGui::Text("Yaw: %.2f", yaw);

        ImGui::Spacing();
        float wid = ImGui::GetContentRegionAvail().x;


        ImGui::Text("Speed:        ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(0.4f * wid);
        ImGui::DragFloat("##speed", &speed, 1.0f, 1.0f, 50.0f, "%.1f");
    
        ImGui::Text("Sensitivity: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(0.4f * wid);
        ImGui::DragFloat("##sensitivity", &sensitivity, 0.01f, 0.01f, 2.0f, "%.1f");


		ImGui::End();
    }
}