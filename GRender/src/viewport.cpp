#include "GRender/viewport.h"

namespace GRender {

Viewport::Viewport(const glm::uvec2& size, const std::vector<texture::Specification>& vSpecs, bool createDepthBuf)
	: Framebuffer(size, vSpecs, createDepthBuf) {}


Viewport::Viewport(Viewport&& vp) noexcept {
	std::swap(m_IsHovered, vp.m_IsHovered);
	std::swap(m_Position, vp.m_Position);
	std::swap(static_cast<Framebuffer&>(*this), static_cast<Framebuffer&>(vp));
}

Viewport& Viewport::operator=(Viewport&& vp) noexcept {
	if (&vp != this) {
		new(this) Viewport(std::move(vp));
	}
	return *this;
}

void Viewport::display(const std::string& windowName, uint32_t attachmentID) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
	ImGui::Begin(windowName.c_str(), NULL, ImGuiWindowFlags_NoTitleBar);

	m_IsHovered = ImGui::IsWindowHovered();

	// Check if it needs to resize
	ImVec2 port = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(uintptr_t) texture(attachmentID).id(), port, { 0.0f, 1.0f }, { 1.0f, 0.0f });

	glm::uvec2 view = size();
	glm::uvec2 uport{ uint32_t(port.x), uint32_t(port.y) };

	if (uport.x != view.x || uport.y != view.y) {
		resize(uport);
	}

	// In case the windows moved
	ImVec2 ps = ImGui::GetWindowPos();
	m_Position = { ps.x,ps.y };

	ImGui::End();
	ImGui::PopStyleVar();
}

} // namespace GRender

