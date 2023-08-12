#include <string>
#include "imgui.h"

#include "events.h"
#include "interactiveImage.h"
#include "texture.h"

namespace GRender {

InteractiveImage::InteractiveImage(const glm::uvec2& size, const texture::Specification& spec, const void* data) 
    : m_Texture(size, spec, data) {

    const float ratio = static_cast<float>(size.x) / static_cast<float>(size.y);
    m_Port = (size.x > size.y) ? glm::vec2{ 1024.0f, 1024.0f / ratio} : glm::vec2{ ratio * 728.0f, 728.0f};
}

InteractiveImage::InteractiveImage(const fs::path& filepath) 
    : m_Texture(utils::createTextureFromRGBAFile(filepath)) {
    
    const glm::uvec2& size = m_Texture.size();
    const float ratio = static_cast<float>(size.x) / static_cast<float>(size.y);
    m_Port = (size.x > size.y) ? glm::vec2{ 1024.0f, 1024.0f / ratio } : glm::vec2{ ratio * 728.0f, 728.0f };
}

InteractiveImage::InteractiveImage(InteractiveImage&& other) noexcept {
    // Simply copying small variables
    m_View = other.m_View;
    m_IsFocused = other.m_IsFocused;
    m_IsHovered = other.m_IsHovered;
    m_Zoom = other.m_Zoom;
    m_PosMin = other.m_PosMin;
    m_PosMax = other.m_PosMax;
    m_Port = other.m_Port;

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
    
    const float titlebarHeight = ImGui::GetFrameHeight();

    // To improve precision, we remove window padding
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize({ m_Port.x, m_Port.y + titlebarHeight}, ImGuiCond_Always);

    // Starting windows
    ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin(windowName.c_str(), &m_View, winFlags);
    
    m_IsHovered = ImGui::IsWindowHovered();
    m_IsFocused = ImGui::IsWindowFocused();

    const ImVec2 ImWinSize = ImGui::GetWindowSize();
    const ImVec2 ImWinPos = ImGui::GetWindowPos();
    const ImVec2 ImMousePos = ImGui::GetMousePos();

    // Getting an up-to-date available content size
    const glm::vec2 portSize = { ImWinSize.x, ImWinSize.y - titlebarHeight };
    
    // What is the biggest image a can put in here with good aspect ratio?
    const glm::vec2 imgSize = ratio > 1.0f ? glm::vec2{ std::min(portSize.x, portSize.y * ratio), portSize.y }
                                           : glm::vec2{ portSize.x, std::min(portSize.y, portSize.x / ratio) };
    
    // Handling window resizing
    const glm::vec2 delta = (imgSize - m_Port) / m_Port;
    if (ratio > 1.0f) {
        m_PosMax.x += (m_PosMax.x - m_PosMin.x) * (delta.x - delta.y);
    }
    else {
        m_PosMin.y -= (m_PosMax.y - m_PosMin.y) * (delta.y - delta.x);
    }

    // Resetting port size for remaining calculations
    m_Port = imgSize;

     // Calculating mouse pointer location from 0->1 in relation to port size or image when port
    // size is too big
    const glm::vec2 pos = { (ImMousePos.x - ImWinPos.x) / m_Port.x, 
                            1.0f - (ImMousePos.y - ImWinPos.y - titlebarHeight) / m_Port.y };

    if (m_IsHovered && pos.x > 0.0f && pos.x < 1.0f && pos.y > 0.0f && pos.y < 1.0f) {
        // Zooming
        const float wheel = GRender::mouse::Wheel();
        if (fabs(wheel) > 0.01f) {
            const float nZoom = m_Zoom + 0.1f * wheel;
            if (nZoom >= 1.0f && nZoom <= 10.0f) {
                const glm::vec2 displayArea = m_PosMax - m_PosMin;
                const glm::vec2 newDisplayArea = (m_Zoom / nZoom) * displayArea;

                m_PosMin += pos * (displayArea - newDisplayArea);
                m_PosMax = m_PosMin + newDisplayArea;
                m_Zoom = nZoom;
            }
        }
        // Panning :: We invert y to conpensate up-down situation
        if (GRender::mouse::IsDown(GRender::MouseButton::LEFT)) {
            const glm::vec2 delta2 = GRender::mouse::Delta() * (m_PosMax - m_PosMin);
            const glm::vec2 deltaPos = { delta2.x / m_Port.x, -delta2.y / m_Port.y };
            m_PosMin -= deltaPos;
            m_PosMax -= deltaPos;
        }
    }

    // We don't want to ever cross the limits of the image, so we are going to lock texture values between 0 and 1
    const glm::vec2 totalArea = { std::min(1.0f, m_PosMax.x - m_PosMin.x), std::min(1.0f, m_PosMax.y - m_PosMin.y) };
    
    if (m_PosMin.x < 0.0f) { m_PosMin.x = 0.0f; m_PosMax.x = totalArea.x; }
    if (m_PosMin.y < 0.0f) { m_PosMin.y = 0.0f; m_PosMax.y = totalArea.y; }
    
    if (m_PosMax.x > 1.0f) { m_PosMax.x = 1.0f; m_PosMin.x = 1.0f - totalArea.x; }
    if (m_PosMax.y > 1.0f) { m_PosMax.y = 1.0f; m_PosMin.y = 1.0f - totalArea.y; }


    // Finally displaying image
    ImGui::Image((void*)(uintptr_t)m_Texture.id(), { m_Port.x, m_Port.y }, {m_PosMin.x, m_PosMax.y}, {m_PosMax.x, m_PosMin.y});
    ImGui::End();

    // Removing styles used in this window
    ImGui::PopStyleVar();
}

} // namespace GRender