#include <string>
#include "imgui.h"

#include "events.h"
#include "interactiveImage.h"
#include "texture.h"

namespace GRender {

InteractiveImage::InteractiveImage(const fs::path& filepath) :
	m_Texture(utils::createTextureFromRGBAFile(filepath)) {}


InteractiveImage::InteractiveImage(InteractiveImage&& other) noexcept {
	std::swap(m_View, other.m_View);
	std::swap(m_IsFocused, other.m_IsFocused);
	std::swap(m_IsHovered, other.m_IsHovered);
	std::swap(m_Zoom, other.m_Zoom);
	std::swap(m_PosMin, other.m_PosMin);
	std::swap(m_PosMax, other.m_PosMax);
	std::swap(m_Padding, other.m_Padding);
	std::swap(m_AspectRatio, other.m_AspectRatio);
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
	if(!m_View) { m_IsFocused = false; return;}

	const glm::uvec2& texSize = m_Texture.size();
	const float ratio = static_cast<float>(texSize.x) / static_cast<float>(texSize.y);

	const float titleHeight = ImGui::GetFrameHeightWithSpacing();
	const ImVec2 winSize = (ratio > 1.0f) ? ImVec2{ 1024.0f, 1024.0f / ratio + titleHeight}
										  : ImVec2{ ratio * 728.0f, 728.0f + titleHeight};

	ImGui::SetNextWindowSize(winSize, ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {m_Padding.x, m_Padding.y});

	// Starting windows
    ImGui::Begin(windowName.c_str(), &m_View);
	m_IsHovered = ImGui::IsWindowHovered();
	m_IsFocused = ImGui::IsWindowFocused();

	// We want to preserve the aspect ratio of our image
	ImVec2 port = ImGui::GetWindowSize();
	port.y -= titleHeight;

	glm::vec2 locSize;
	const float portRatio = port.x / port.y;

	if (ratio >= 1.0f) {
		if (portRatio > ratio) {
			locSize = { texSize.x * port.y / texSize.y, port.y };
			m_Padding = { 0.5f * (port.x - locSize.x), 0.0f };
		}
		else {
			const float rat = portRatio / ratio;
			const float dRatio = m_AspectRatio.x - rat;
			m_AspectRatio.x = rat;

			locSize = { port.x, port.y };
			m_Padding = { 0.0f, 0.0f };
			m_Zoom.x -= dRatio;
			m_PosMin.x += 0.5f * dRatio;
		}
	}
	else {
		if (portRatio > ratio) {
			const float rat = ratio / portRatio;
			const float dRatio = m_AspectRatio.y - rat;
			m_AspectRatio.y = rat;

			locSize = { port.x, port.y };
			m_Padding = { 0.0f, 0.0f };
			m_Zoom.y -= dRatio;
			m_PosMin.y += 0.5f * dRatio;
		}
		else {
			locSize = { port.x, texSize.y * port.x / texSize.x };
			m_Padding = { 0.0f, 0.5f * (port.y - locSize.y) };
		}
	}

	// Calculating screen position
	ImVec2 winpos = ImGui::GetWindowPos();
	winpos.y += titleHeight;

	glm::vec2 pos = GRender::mouse::Position();
	pos.x = (pos.x - winpos.x - m_Padding.x) / locSize.x;
	pos.y = 1.0f - (pos.y - winpos.y - m_Padding.y) / locSize.y;

	// // We only want action to take place if mouse is on top of image
	bool onTop = m_IsHovered && pos.x > 0.0f && pos.x < 1.0f && pos.y > 0.0f && pos.y < 1.0f;
	if (onTop) {

		// Zooming
		float dZoom = -0.1f * GRender::mouse::Wheel();
		if (fabs(dZoom) > 0.001) {
			//Calculating new zoom
			glm::vec2 nZoom = m_Zoom + dZoom * m_AspectRatio;
			nZoom.x = std::max(std::min(nZoom.x, m_AspectRatio.x), 0.1f * m_AspectRatio.x);
			nZoom.y = std::max(std::min(nZoom.y, m_AspectRatio.y), 0.1f * m_AspectRatio.y);

			// Estimating new m_PosMin
			pos = m_PosMin + m_Zoom * pos;
			m_PosMin = pos - (nZoom / m_Zoom) * (pos - m_PosMin);
			m_Zoom = nZoom;
		}

		// Panning
		if (GRender::mouse::IsDown(GRender::MouseButton::LEFT)) {
			glm::vec2 dr = m_Zoom * GRender::mouse::Delta() / locSize;
			m_PosMin.x -= dr.x;
			m_PosMin.y += dr.y;
		}
	}

	// Setting up constrains for imaged displayed
	m_PosMin.x = std::max(m_PosMin.x + m_Zoom.x > 1.0f ? 1.0f - m_Zoom.x : m_PosMin.x, 0.0f);
	m_PosMin.y = std::max(m_PosMin.y + m_Zoom.y > 1.0f ? 1.0f - m_Zoom.y : m_PosMin.y, 0.0f);
	m_PosMax = m_PosMin + m_Zoom;

	// Finally displaying image
	ImGui::Image((void*)(uintptr_t)m_Texture.id(), {locSize.x, locSize.y}, {m_PosMin.x, m_PosMax.y}, {m_PosMax.x, m_PosMin.y});
    ImGui::End();

    ImGui::PopStyleVar();
}

} // namespace GRender