#pragma once

#include <vector>
#include "framebuffer.h"

namespace GRender {

class Viewport : public Framebuffer {
public:
	Viewport(void) = default;
	~Viewport(void) = default;

	Viewport(const glm::uvec2& size,
			 const std::vector<texture::Specification>& vSpecs = { texture::Specification() },
			 bool createDepthBuf = false);

	// We don't want a viewport to be copied
	Viewport(const Viewport&) = delete;
	Viewport& operator=(const Viewport&) = delete;

	// But it can be moved, no problem
	Viewport(Viewport&& fBuffer) noexcept;
	Viewport& operator=(Viewport&& fBuffer) noexcept;

	// The viewport properties are controlled by the UI
	bool hovered(void) const { return m_IsHovered; }
	glm::uvec2 position(void) const { return m_Position; }

	// Creates a ImGui Windows and display attached framebuffer
	void display(const std::string& windowName = "Viewport", uint32_t attachementID = 0);

private:
	bool m_IsHovered = false;
	glm::uvec2 m_Position = { 0,0 };
};

} // namespace GRender
