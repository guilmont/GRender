#include "fonts.h"

#include <filesystem>

void Fonts::setDefault(const std::string &name)
{
    ImGui::GetIO().FontDefault = mFonts[name];
}

void Fonts::loadFont(const std::string &fontname, const std::string &path, float size)
{
    mFonts[fontname] = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size);
}

void Fonts::text(const std::string &txt, const std::string &fontname)
{
    ImGui::PushFont(mFonts[fontname]);
    ImGui::Text("%s", txt.c_str());
    ImGui::PopFont();
}

void Fonts::push(const std::string &fontname)
{
    ImGui::PushFont(mFonts[fontname]);
}

void Fonts::pop(void)
{
    ImGui::PopFont();
}
