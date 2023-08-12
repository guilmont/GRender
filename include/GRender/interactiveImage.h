#pragma once

#include "core.h"
#include "texture.h"

namespace GRender {

class InteractiveImage {
public:
	InteractiveImage(const glm::uvec2& size, const texture::Specification& spec = texture::Specification(), const void* data = nullptr);
	InteractiveImage(const fs::path& filepath);
	InteractiveImage(void) = default;
	~InteractiveImage(void) = default;

	Texture& texture() { return m_Texture; }
	const Texture& texture() const { return m_Texture; }

	// We don't want it to be copied
	InteractiveImage(const InteractiveImage&) = delete;
	InteractiveImage& operator=(const InteractiveImage&) = delete;

	// But it can be moved, no problem
	InteractiveImage(InteractiveImage&&) noexcept;
	InteractiveImage& operator=(InteractiveImage&&) noexcept;

	// Creates a ImGui Window and display attached Texture
	void display(const std::string& windowName = "Interactive Image");

	void open(void) { m_View = true; }
	void close(void) { m_View = false; }

	// Checks if the window is opened to view by user
	bool isOpened(void) const { return m_View; }
	bool isFocused(void) const { return m_IsFocused; }
	bool isHovered(void) const { return m_IsHovered; }

private:
	bool m_View = true;
	bool m_IsFocused = false;
	bool m_IsHovered = false;

	float m_Zoom = 1.0f;
	glm::vec2 m_Port = { 1.0f, 1.0f };

	// Texture coordinates to use with ImGui::Image
	glm::vec2 m_PosMin = {0.0f, 0.0f};
	glm::vec2 m_PosMax = {1.0f, 1.0f};

	Texture m_Texture;
};

} // namespace GRender
