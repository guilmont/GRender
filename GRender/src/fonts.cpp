#include "fonts.h"

namespace GRender
{

    void Fonts::setDefault(const std::string& name) { ImGui::GetIO().FontDefault = mFonts[name]; }

    void Fonts::swap(const std::string& name1, const std::string& name2) { std::swap(mFonts[name1], mFonts[name2]); }

    void Fonts::loadFont(const std::string& fontname, const std::string& path, float size) { mFonts[fontname] = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size); }

    void Fonts::text(const std::string& txt, const std::string& fontname)
    {
        ImGui::PushFont(mFonts[fontname]);
        ImGui::Text("%s", txt.c_str());
        ImGui::PopFont();
    }

    void Fonts::push(const std::string& fontname) { ImGui::PushFont(mFonts[fontname]); }

    void Fonts::pop(void) { ImGui::PopFont(); }

}