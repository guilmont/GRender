#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "events.h"
#include "fonts.h"
#include "utils.h"

namespace GRender {

Camera::Camera(const glm::vec3& defPos, float defPitch, float defYaw)
: m_Angles({defYaw, defPitch}), m_DefAngles(m_Angles), m_Position(defPos), m_DefPosition(defPos) {
    calculateFront();
}

void Camera::open()  { m_Active = true;  }
void Camera::close() { m_Active = false; }

void Camera::display(void) {
    if (!m_Active) { return; }

    ImGui::Begin("Camera info", &m_Active);
    ImGui::SetWindowSize({ 500.0f, 225.0f }, ImGuiCond_FirstUseEver);

    float space = 0.25f;
    float drag = 0.333f;

    utils::Drag<float,3>("Position:", m_Position, space, 3.08f * drag, 0.1f);

    float value = glm::degrees(m_Angles.y);
    if (utils::Drag("Pitch:", value, space, drag, 0.25f, -89.0f, 89.0f, "%.f")) {
        value = std::max(std::min(value, 89.0f), -89.0f);
        m_Angles.y = glm::radians(value);
    }

    value = glm::degrees(m_Angles.x);
    if (utils::Drag("Yaw:", value, space, drag, 0.25f, -180.0f, 180.0f, "%.f")) {
        m_Angles.x = glm::radians(value);
    }

    ImGui::Dummy({0.0f, 5.0f});
    if (ImGui::Button("Set defaults")) {
        m_DefPosition = m_Position;
        m_DefAngles = m_Angles;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        reset();

    ///////////////////////////////////////////////////////

    space += 0.1f; // To account for indentation

    ImGui::Dummy({ 0.0f, 10.0f });

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;
    nodeFlags |= ImGuiTreeNodeFlags_Framed;
    //nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;

    if (ImGui::TreeNodeEx("Configurations", nodeFlags)) {
        utils::Drag("Sensitivity:", m_Sensitivity, space, drag, 0.01f, 0.01f, 2.0f, "%.2f");

        utils::Drag("Speed:", m_Speed, space, drag, 1.0f, 1.0f, 50.0f, "%.1f");

        value = glm::degrees(m_FOV);
        if (utils::Drag("FOV:", value, space, drag, 0.5f, 15.0f, 60.0f, "%.f"))
            m_FOV = glm::radians(value);

        ImGui::Dummy({ 0.0f, 5.0f });

        ImGui::Unindent();
        if (ImGui::TreeNode("Defaults")) {
            ImGui::Dummy({0.0f, 5.0f});
            ImGui::PushID("defaults");

            utils::Drag<float, 3>("Position:", m_DefPosition, space, 3.08f * drag, 0.1f);

            value = glm::degrees(m_DefAngles.y);
            if (utils::Drag("Pitch:", value, space, drag, 0.25f, -89.0f, 89.0f, "%.f")) {
                m_DefAngles.y = glm::radians(value);
            }

            value = glm::degrees(m_DefAngles.x);
            if (utils::Drag("Yaw:", value,  space, drag, 0.25f, -180.0f, 180.0f, "%.f")) {
                m_DefAngles.x = glm::radians(value);
            }

            ImGui::PopID();
            ImGui::Dummy({ 0.0f, 10.0f });
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

glm::mat4 Camera::getViewMatrix(void) {
    glm::mat4 proj = glm::perspective(m_FOV, m_Ratio, m_Near, m_Far);
    glm::mat4 view = glm::lookAt(m_Position, m_Position + m_Front, {0.0f, 1.0f, 0.0f });
    return proj * view;
}

void Camera::reset() {
    m_Position = m_DefPosition;
    m_Angles = m_DefAngles;
    calculateFront();
}

void Camera::controls(float deltaTime) {
    if (keyboard::IsDown('W') || (mouse::Wheel() > 0.0f)) { m_Position += m_Front * m_Speed * deltaTime; }
    if (keyboard::IsDown('S') || (mouse::Wheel() < 0.0f)) { m_Position -= m_Front * m_Speed * deltaTime; }

    if (keyboard::IsDown('E')) { m_Position.y += m_Speed * deltaTime; }
    if (keyboard::IsDown('Q')) { m_Position.y -= m_Speed * deltaTime; }

    if (keyboard::IsDown('D')) { m_Position += m_Side * m_Speed * deltaTime; }
    if (keyboard::IsDown('A')) { m_Position -= m_Side * m_Speed * deltaTime; }

    if (mouse::IsClicked(MouseButton::MIDDLE)) { reset(); }
    if (mouse::IsDown(MouseButton::LEFT)) { lookAround(mouse::Delta() * m_Sensitivity * deltaTime); }
}


void Camera::calculateFront() {
    m_Front = { cos(m_Angles.x) * cos(m_Angles.y), sin(m_Angles.y), sin(m_Angles.x) * cos(m_Angles.y) };
    m_Side = { -m_Front.z, 0.0f, m_Front.x };
}

void Camera::lookAround(const glm::vec2& offset) {
    m_Angles.x -= offset.x;
    m_Angles.y += offset.y;

    const float lim = glm::radians(89.0f);
    m_Angles.y = std::max(std::min(m_Angles.y, lim), - lim);

    calculateFront();
}

} // namespace GRender
