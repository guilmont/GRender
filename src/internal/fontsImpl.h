#pragma once

#include "core.h"

namespace GRender::fonts::internal {

class FontsImpl {
public:
	static FontsImpl* Instance();
	
	FontsImpl(const FontsImpl&) = delete;
	FontsImpl& operator=(const FontsImpl&) = delete;

	void loadFont(const std::string& fontname, const std::string& path, float size);
	void setDefault(const std::string& name);

	// Used for DPI switching
	void swap(const std::string& name1, const std::string& name2);

	void text(const std::string& txt, const std::string& type);
	void push(const std::string& fontname);
	void pop(void);

private:
	FontsImpl(void);
	~FontsImpl(void) = default;

	std::unordered_map<std::string, ImFont*> mFonts;
};

} // namespace GRender::fonts::internal