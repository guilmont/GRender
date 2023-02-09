#include <glm/gtc/matrix_transform.hpp>

#include "events.h"
#include "orbitalCamera.h"
#include "utils.h"

namespace GRender {

OrbitalCamera::OrbitalCamera(const glm::vec3& center, float distance, float theta, float phi)
: Camera(center, theta, phi), m_Distance(distance), m_DefDistance(distance) {}

void OrbitalCamera::display(void) {
    if (!m_Active) { return; }

    ImGui::Begin("Orbital camera", &m_Active);
    ImGui::SetWindowSize({ DPI_FACTOR * 500.0f, DPI_FACTOR * 225.0f }, ImGuiCond_FirstUseEver);
    
    float split = 0.25f, boxWidth = 0.333f;

    utils::Drag<float,3>("Center:", m_Position, split, 3.08f * boxWidth, 0.1f);
    utils::Drag("Distance:", m_Distance, split, boxWidth, 0.1f);

    float value = glm::degrees(m_Angles.x);
    if (utils::Drag("Phi:", value, split, boxWidth, 0.25f, -180.0f, 180.0f, "%.f")) {
        m_Angles.x = glm::radians(value);
    }
    
    value = glm::degrees(m_Angles.y);
    if (utils::Drag("Theta:", value, split, boxWidth, 0.25f, -89.0f, 89.0f, "%.f")) {
        value = std::max(std::min(value, 89.0f), -89.0f);
        m_Angles.y = glm::radians(value);
    }

    ImGui::Dummy({0.0f, 5.0f * DPI_FACTOR});

    if (ImGui::Button("Set defaults")) {
        m_DefDistance = m_Distance;
        m_DefAngles = m_Angles;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        reset();

    ///////////////////////////////////////////////////////

    split += 0.1f;
    ImGui::Dummy({ 0.0f, 10.0f * DPI_FACTOR });
 
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_None;
    nodeFlags |= ImGuiTreeNodeFlags_Framed;
    //nodeFlags |= ImGuiTreeNodeFlags_AllowItemOverlap;

    if (ImGui::TreeNodeEx("Configurations", nodeFlags)) {
        utils::Drag("Sensitivity:", m_Sensitivity, split, boxWidth, 0.01f, 0.01f, 2.0f, "%.2f");
        utils::Drag("Speed:", m_Speed, split, boxWidth, 0.5f, 1.0f, 50.0f, "%.1f");

        value = glm::degrees(m_FOV);
        if (utils::Drag("FOV:", value, split, boxWidth, 0.5f, 15.0f, 60.0f, "%.f"))
            m_FOV = glm::radians(value);
        
        ImGui::Dummy({ 0.0f, 5.0f * DPI_FACTOR });
    
        ImGui::Unindent();
        if (ImGui::TreeNode("Defaults")) {
            ImGui::Dummy({0.0f, 5.0f * DPI_FACTOR});
            ImGui::PushID("defaults");

            utils::Drag<float, 3>("Center:", m_DefPosition, split, 3.08f * boxWidth, 0.1f);
            utils::Drag("Distance", m_DefDistance, split, boxWidth, 0.1f);

            value = glm::degrees(m_DefAngles.y);
            if (utils::Drag("Theta", value, split, boxWidth, 0.25f, -89.0f, 89.0f, "%.f")) {
                m_DefAngles.y = glm::radians(value);
            }

            value = glm::degrees(m_DefAngles.x);
            if (utils::Drag("Phi:", value, split, boxWidth, 0.25f, -180.0f, 180.0f, "%.f")) {
                m_DefAngles.x = glm::radians(value);
            }
            
            ImGui::PopID();
            ImGui::Dummy({ 0.0f, 10.0f * DPI_FACTOR });
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
    ImGui::End();
}

glm::mat4 OrbitalCamera::getViewMatrix(void) {
    glm::mat4 proj = glm::perspective(m_FOV, m_Ratio, m_Near, m_Far);
    glm::mat4 view = glm::lookAt(m_Distance * m_Front, m_Position, {0.0f, 1.0f, 0.0f });
    return proj * view;
}

void OrbitalCamera::reset() {
    Camera::reset();
    m_Distance = m_DefDistance;
}

void OrbitalCamera::controls(float deltaTime) {
    if (keyboard::IsDown('W') || (mouse::Wheel() > 0.0f)) { m_Distance -= m_Speed * deltaTime; } 
    if (keyboard::IsDown('S') || (mouse::Wheel() < 0.0f)) { m_Distance += m_Speed * deltaTime; }

    if (keyboard::IsDown('E')) { orbitOrigin(glm::vec2{0.0f, +0.1f} * m_Speed * deltaTime); }
    if (keyboard::IsDown('Q')) { orbitOrigin(glm::vec2{0.0f, -0.1f} * m_Speed * deltaTime); }

    if (keyboard::IsDown('D')) { orbitOrigin(glm::vec2{-0.1f, 0.0f} * m_Speed * deltaTime); }
    if (keyboard::IsDown('A')) { orbitOrigin(glm::vec2{+0.1f, 0.0f} * m_Speed * deltaTime); }

    if (mouse::IsClicked(MouseButton::MIDDLE)) { reset(); }
    if (mouse::IsDown(MouseButton::LEFT)) { orbitOrigin(mouse::Delta() * m_Sensitivity * deltaTime); }
}

void OrbitalCamera::orbitOrigin(const glm::vec2& offset) {
    m_Angles += offset;

    const float lim = glm::radians(89.0f);
    m_Angles.y = std::max(std::min(m_Angles.y, lim), - lim);

    calculateFront();
}

} // namespace GRender
