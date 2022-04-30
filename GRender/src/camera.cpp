#include "camera.h"
#include "fonts.h"

#include <glm/gtc/matrix_transform.hpp>

namespace GRender {

Camera::Camera(const glm::vec3& defPos, const glm::vec3& defFront) : mDefaultPosition(defPos), mPosition(defPos) {
    mDefaultFront = glm::normalize(defFront);
    mFront = mDefaultFront;

    mPitch = glm::asin(mFront.y);
    mYaw = glm::atan(mFront.z/mFront.x);
}

void Camera::open() {
    active = true;
}

void Camera::close() {
    active = false;
}

void Camera::showControls(void) {
    if (!active) {
        return;
    }

    ImGui::Begin("Camera info", &active);
    ImGui::SetWindowSize({ DPI_FACTOR * 350.0f, DPI_FACTOR * 200.0f });
    
    fonts::Text("Position:", "bold");
    ImGui::SameLine();
    ImGui::Text("%.2f x %.2f x %.2f", mPosition.x, mPosition.y, mPosition.z);

    fonts::Text("Front:", "bold");
    ImGui::SameLine();
    ImGui::Text("%.2f : %.2f : %.2f", mFront.x, mFront.y, mFront.z);

    fonts::Text("Pitch:", "bold");
    ImGui::SameLine();
    ImGui::Text("%.1f degrees", glm::degrees(mPitch));

    fonts::Text("Yaw:", "bold");
    ImGui::SameLine();
    ImGui::Text("%.1f degrees", glm::degrees(mYaw));

    ImGui::Spacing();
    float wid = ImGui::GetContentRegionAvail().x;


    fonts::Text("Speed:", "bold");
    ImGui::SameLine(5.0f*ImGui::GetFontSize());
    ImGui::SetNextItemWidth(0.4f * wid);
    ImGui::DragFloat("##Speed", &mSpeed, 1.0f, 1.0f, 50.0f, "%.1f");

    fonts::Text("Sensitivity:", "bold");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(0.4f * wid);
    ImGui::DragFloat("##Sensitivity", &mSensitivity, 0.01f, 0.01f, 2.0f, "%.1f");

    ImGui::End();
}

glm::mat4 Camera::getViewMatrix(void) {
    glm::mat4 proj = glm::perspective(mFOV, mRatio, mNear, mFar);
    glm::mat4 view = glm::lookAt(mPosition, mPosition + mFront, {0.0f, 1.0f, 0.0f });
    return proj * view;
}

void Camera::reset() {
    bool view = active;
    new(this) Camera(mDefaultPosition, mDefaultFront);
    active = view;
}

void Camera::moveFront(float elapsed) {
    mPosition += mFront * mSpeed * elapsed;
}

void Camera::moveBack(float elapsed) {
    mPosition -= mFront * mSpeed * elapsed;
}

void Camera::moveUp(float elapsed) {
    mPosition.y += mSpeed * elapsed;
}

void Camera::moveDown(float elapsed) {
    mPosition.y -= mSpeed * elapsed;
}
    
void Camera::moveLeft(float elapsed) {
    mPosition -= mSpeed * elapsed * glm::vec3{ -mFront.z, 0.0f, mFront.x };
}

void Camera::moveRight(float elapsed) {
    mPosition += mSpeed * elapsed * glm::vec3{ -mFront.z, 0.0f, mFront.x };
}

void Camera::lookAround(const glm::vec2& offset, float elapsed) {
    mYaw -= offset.x * mSensitivity * elapsed;
    mPitch += offset.y * mSensitivity * elapsed;

    mPitch = std::max(std::min(mPitch, 1.0f), -1.0f);
    mFront = glm::normalize(glm::vec3{cos(mYaw) * cos(mPitch), sin(mPitch), sin(mYaw) * cos(mPitch)});
}


float& Camera::aspectRatio() {
    return mRatio;
}

float& Camera::yaw() {
    return mYaw;
}

float& Camera::pitch() {
    return mPitch;
}
glm::vec3& Camera::position() {
    return mPosition;
}

glm::vec3 Camera::front() const {
    return mFront;
}

void Camera::setDefaultPosition(const glm::vec3& pos) {
    mDefaultPosition = pos;
}

void Camera::setDefaultFront(const glm::vec3& front) {
    mDefaultFront = front;
}

} // namespace GRender