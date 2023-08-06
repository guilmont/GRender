#include <string>
#include "imgui.h"

#include "events.h"
#include "interactiveImage.h"
#include "texture.h"

namespace GRender {

std::ostream& operator<<(std::ostream& out, const glm::vec2& var) {
    out << var.x << ',' << var.y;
    return out;
}

InteractiveImage::InteractiveImage(const glm::uvec2& size, const texture::Specification& spec, const void* data) : m_Texture(size, spec, data) {}

InteractiveImage::InteractiveImage(const fs::path& filepath) : m_Texture(utils::createTextureFromRGBAFile(filepath)) {}

InteractiveImage::InteractiveImage(InteractiveImage&& other) noexcept {
    // Simply copying small variables
    m_View = other.m_View;
    m_IsFocused = other.m_IsFocused;
    m_IsHovered = other.m_IsHovered;
    m_Zoom = other.m_Zoom;
    m_PosMin = other.m_PosMin;
    m_PosMax = other.m_PosMax;

    // We don't want texture to be copied
    std::swap(m_Texture, other.m_Texture);
}

InteractiveImage& InteractiveImage::operator=(InteractiveImage && other) noexcept {
    if (this != &other) {
        this->~InteractiveImage();
        new (this)InteractiveImage(std::move(other));
    }
    return *this;
}

void InteractiveImage::display(const std::string& windowName) {
    if (!m_View) { return; }

    const glm::uvec2& texSize = m_Texture.size();
    const float ratio = static_cast<float>(texSize.x) / static_cast<float>(texSize.y);

    const float titleHeight = ImGui::GetFrameHeightWithSpacing();
    const ImVec2 firstWinSize = (ratio > 1.0f) ? ImVec2{ 1024.0f, 1024.0f / ratio + titleHeight}
                                         : ImVec2{ ratio * 728.0f, 728.0f + titleHeight };

    // Starting windows
    ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin(windowName.c_str(), &m_View, winFlags);
    ImGui::SetWindowSize(firstWinSize, ImGuiCond_Once);

    m_IsHovered = ImGui::IsWindowHovered();
    m_IsFocused = ImGui::IsWindowFocused();

    // We need to find the viewport size and relative mouse position
    const ImVec2 ImWinSize = ImGui::GetWindowSize();
    const ImVec2 ImWinPos = ImGui::GetWindowPos();
    const ImVec2 ImMousePos = ImGui::GetMousePos();

    const glm::vec2 portSize = { ImWinSize.x, ImWinSize.y - titleHeight };
    const glm::vec2 pos = { (ImMousePos.x - ImWinPos.x) / portSize.x, 
                            1.0f - (ImMousePos.y - ImWinPos.y - titleHeight) / portSize.y };

    if (m_IsHovered && pos.x > 0.0f && pos.x < 1.0f && pos.y > 0.0f && pos.y < 1.0f) {
        // Zooming
        const float wheel = GRender::mouse::Wheel();
        if (fabs(wheel) > 0.001f) {
            const float nZoom = m_Zoom + 0.1f * wheel;
            if (nZoom >= 1.0f && nZoom <= 10.0f) {
                m_PosMin += pos * (1.0f/m_Zoom - 1.0f/ nZoom);
                m_PosMax = m_PosMin + glm::vec2(1.0f / nZoom);
                m_Zoom = nZoom;
            }
        }
        // Panning :: We invert y to conpensate up-down situation
        if (GRender::mouse::IsDown(GRender::MouseButton::LEFT)) {
            const glm::vec2 delta = GRender::mouse::Delta() / m_Zoom;
            const glm::vec2 deltaPos = { delta.x / portSize.x, -delta.y / portSize.y };
            m_PosMin -= deltaPos;
            m_PosMax -= deltaPos;
        }
    }
    // We don't want to ever cross the limits of the image, so we are going to lock values
    // between 0 and 1
    if (m_PosMin.x < 0.0f)      { m_PosMin.x = 0.0f; m_PosMax.x = 1 / m_Zoom;           }
    else if (m_PosMax.x > 1.0f) { m_PosMax.x = 1.0f; m_PosMin.x = 1.0f - 1.0f / m_Zoom; }

    if (m_PosMin.y < 0.0f)      { m_PosMin.y = 0.0f; m_PosMax.y = 1 / m_Zoom;           }
    else if (m_PosMax.y > 1.0f) { m_PosMax.y = 1.0f; m_PosMin.y = 1.0f - 1.0f / m_Zoom; }

    // Finally displaying image
    const ImVec2 imgSize = { portSize.y * ratio, portSize.y };
    ImGui::Image((void*)(uintptr_t)m_Texture.id(), imgSize, {m_PosMin.x, m_PosMax.y}, {m_PosMax.x, m_PosMin.y});
    ImGui::End();

}

} // namespace GRender