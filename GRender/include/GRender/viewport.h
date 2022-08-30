#pragma once

#include <vector>
#include "framebuffer.h"

namespace GRender {

class Viewport {
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

	// Utilities to bind and unbing attached framebuffer
	void bind() const { m_Buffer.bind(); }
	void unbind() const { m_Buffer.unbind(); }

	// The viewport properties are controlled by the UI
	bool hovered(void) const { return m_IsHovered; }

	const glm::uvec2& position(void) const { return m_Position; }
	const glm::uvec2& size(void) const { return m_Buffer.size(); }

	// Creates a ImGui Windows and display attached framebuffer
	void display(const std::string& windowName = "Viewport", uint32_t attachementID = 0);

private:
	Framebuffer m_Buffer;

	bool m_IsHovered = false;
	glm::uvec2 m_Position = { 0,0 };
};

} // namespace GRender
