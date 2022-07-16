#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "events.h"
#include "fonts.h"

namespace GRender {

Camera::Camera(const glm::vec3& defPos, float defPitch, float defYaw) : 
    mDefPosition(defPos), mPosition(defPos),
    mDefPitch(defPitch), mPitch(defPitch),
    mDefYaw(defYaw), mYaw(defYaw) {

    calculateFront();
}

void Camera::open() {
    active = true;
}

void Camera::close() {
    active = false;
}

void Camera::display(void) {
    if (!active) {
        return;
    }

    ImGui::Begin("Camera info", &active);
    ImGui::SetWindowSize({ DPI_FACTOR * 500.0f, DPI_FACTOR * 225.0f }, ImGuiCond_FirstUseEver);
    
    float space = 0.7f * ImGui::GetContentRegionAvail().x;
    float drag = 0.33f * space;


    auto spaceWidth = [](float space, float width) {
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - space);
        ImGui::SetNextItemWidth(width);
    };

    fonts::Text("Position:", "bold");
    spaceWidth(space, 3.03f * drag);
    ImGui::DragFloat3("##Position", &mPosition[0], 0.1f);

    float value = glm::degrees(mPitch);
    fonts::Text("Pitch:", "bold");
    spaceWidth(space, drag);
    if (ImGui::DragFloat("##Pitch", &value, 0.25f, -89.0f, 89.0f, "%.f")) {
        value = std::max(std::min(value, 89.0f), -89.0f);
        setPitch(glm::radians(value));
    }

    value = glm::degrees(mYaw);
    fonts::Text("Yaw:", "bold");
    spaceWidth(space, drag);
    if (ImGui::DragFloat("##Yaw", &value, 0.25f, -180.0f, 180.0f, "%.f")) {
        setYaw(glm::radians(value));
    }

    ImGui::Dummy({0.0f, 5.0f * DPI_FACTOR});
    if (ImGui::Button("Set defaults")) {
        mDefPosition = mPosition;
        mDefPitch = mPitch;
        mDefYaw = mYaw;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        reset();

    ///////////////////////////////////////////////////////

    space = 0.65f * ImGui::GetContentRegionAvail().x;

    ImGui::Dummy({ 0.0f, 10.0f * DPI_FACTOR });
 
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;
    nodeFlags |= ImGuiTreeNodeFlags_Framed;
    //nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;

    if (ImGui::TreeNodeEx("Configurations", nodeFlags)) {
        ImGui::Text("Sensitivity:");
        spaceWidth(space, drag);
        ImGui::DragFloat("##Sensitivity", &mSensitivity, 0.01f, 0.01f, 2.0f, "%.2f");

        ImGui::Text("Speed:");
        spaceWidth(space, drag);
        ImGui::DragFloat("##Speed", &mSpeed, 1.0f, 1.0f, 50.0f, "%.1f");

        value = glm::degrees(mFOV);
        ImGui::Text("FOV:");
        spaceWidth(space, drag);
        if (ImGui::DragFloat("##FOV", &value, 0.5f, 15.0f, 60.0f, "%.f"))
            mFOV = glm::radians(value);
        
        ImGui::Dummy({ 0.0f, 5.0f * DPI_FACTOR });
    
        ImGui::Unindent();
        if (ImGui::TreeNode("Defaults")) {
            ImGui::Dummy({0.0f, 5.0f * DPI_FACTOR});

            ImGui::Text("Position:");
            spaceWidth(space, 3.05f * drag);
            ImGui::DragFloat3("##defPosition", &mDefPosition[0], 0.1f);

            value = glm::degrees(mDefPitch);
            ImGui::Text("Pitch:");
            spaceWidth(space, drag);
            if (ImGui::DragFloat("##defPitch", &value, 0.25f, -89.0f, 89.0f, "%.f")) {
                mDefPitch = glm::radians(value);
            }

            value = glm::degrees(mDefYaw);
            ImGui::Text("Yaw:");
            spaceWidth(space, drag);
            if (ImGui::DragFloat("##defYaw", &value, 0.25f, -180.0f, 180.0f, "%.f")) {
                mDefYaw = glm::radians(value);
            }
            
            ImGui::Dummy({ 0.0f, 10.0f * DPI_FACTOR });
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }


    ImGui::End();
}

glm::mat4 Camera::getViewMatrix(void) {
    glm::mat4 proj = glm::perspective(mFOV, mRatio, mNear, mFar);
    glm::mat4 view = glm::lookAt(mPosition, mPosition + mFront, {0.0f, 1.0f, 0.0f });
    return proj * view;
}

void Camera::reset() {
    mPosition = mDefPosition;
    mPitch = mDefPitch;
    mYaw = mDefYaw;

    calculateFront();
}

void Camera::controls(float deltaTime) {
    if (keyboard::IsDown('W') || (mouse::Wheel() > 0.0f))
        moveFront(deltaTime);

    if (keyboard::IsDown('S') || (mouse::Wheel() < 0.0f))
        moveBack(deltaTime);

    if (keyboard::IsDown('D'))
        moveRight(deltaTime);

    if (keyboard::IsDown('A'))
        moveLeft(deltaTime);

    if (keyboard::IsDown('E'))
        moveUp(deltaTime);

    if (keyboard::IsDown('Q'))
        moveDown(deltaTime);

    if (mouse::IsClicked(GRender::MouseButton::MIDDLE))
        reset();

    if (mouse::IsDown(GRender::MouseButton::LEFT))
        lookAround(mouse::Delta(), deltaTime);
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

void Camera::calculateFront() {
    mFront = glm::normalize(glm::vec3{ cos(mYaw) * cos(mPitch), sin(mPitch), sin(mYaw) * cos(mPitch) });
}

void Camera::lookAround(const glm::vec2& offset, float elapsed) {
    mYaw -= offset.x * mSensitivity * elapsed;

    float lim = 89.0f / 180.0f * 3.1415926f;
    mPitch += offset.y * mSensitivity * elapsed;
    mPitch = std::max(std::min(mPitch, lim), - lim);

    calculateFront();
}

const glm::vec3& Camera::getPosition() const { return mPosition; }
void Camera::setPosition(const glm::vec3& pos) { mPosition = pos; }

float Camera::getFOV() const { return mFOV; }
void Camera::setFOV(float fov) { mFOV = fov; }

float Camera::getSensitivity() const { return mSensitivity; }
void Camera::setSensitivity(float sensitivity) { mSensitivity = sensitivity; }

float Camera::getSpeed() const { return mSpeed; }
void Camera::setSpeed(float speed) { mSpeed = speed; }

float Camera::getAspectRatio() const { return mRatio; }
void Camera::setAspectRatio(float aRatio) { mRatio = aRatio; }

float Camera::getYaw() const { return mYaw; }
void Camera::setYaw(float value) { mYaw = value; calculateFront(); }

float Camera::getPitch() const { return mPitch; }
void Camera::setPitch(float value) { mPitch = value; calculateFront(); }


const glm::vec3& Camera::getDefaultPosition() const { return mDefPosition; }
void Camera::setDefaultPosition(const glm::vec3& pos) { mDefPosition = pos; }

float Camera::getDefaultPitch() const { return mDefPitch; }
void Camera::setDefaultPitch(float pitch) { mDefPitch = pitch; }

float Camera::getDefaultYaw() const { return mDefYaw; }
void Camera::setDefaultYaw(float yaw) { mDefYaw = yaw; }

} // namespace GRender