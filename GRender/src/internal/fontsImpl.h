#pragma once

#include "core.h"

namespace GRender::fonts::internal {

class FontsImpl {
public:
	FontsImpl(void);
	~FontsImpl(void) = default;

	void loadFont(const std::string& fontname, const std::string& path, float size);
	void setDefault(const std::string& name);

	// Used for DPI switching
	void swap(const std::string& name1, const std::string& name2);

	void text(const std::string& txt, const std::string& type);
	void push(const std::string& fontname);
	void pop(void);

private:
	std::unordered_map<std::string, ImFont*> mFonts;
};

} // namespace GRender::fonts::internal