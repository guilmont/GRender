#include "fonts.h"
#include "OpenSans.cpp"

namespace GRender {

void Fonts::loadDefaultFonts() {
    const float fontSize = 18.0f;
    mFonts["regular"] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansRegular_compressed_data_base85, fontSize);
    mFonts["bold"] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansBold_compressed_data_base85, fontSize);
    mFonts["italic"] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansItalic_compressed_data_base85, fontSize);

    const float fontDPI = 2.0f * fontSize;
    mFonts["regularDPI"] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansRegular_compressed_data_base85, fontDPI);
    mFonts["boldDPI"] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansBold_compressed_data_base85, fontDPI);
    mFonts["italicDPI"] = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSansItalic_compressed_data_base85, fontDPI);

    setDefault("regular");
}

void Fonts::setDefault(const std::string& name) {
    ASSERT(mFonts.find(name) != mFonts.end(), "Font name not found => " + name);
    ImGui::GetIO().FontDefault = mFonts[name];
}

void Fonts::swap(const std::string& name1, const std::string& name2) {
    ASSERT(mFonts.find(name1) != mFonts.end(), "Font name not found => " + name1);
    ASSERT(mFonts.find(name2) != mFonts.end(), "Font name not found => " + name2);
    std::swap(mFonts[name1], mFonts[name2]);
}

void Fonts::loadFont(const std::string& fontname, const std::string& path, float size) {
    ASSERT(mFonts.find(fontname) == mFonts.end(), "Font name already exists!!");
    mFonts[fontname] = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), size);
}

void Fonts::text(const std::string& txt, const std::string& fontname) {
    ImGui::PushFont(mFonts[fontname]);
    ImGui::Text("%s", txt.c_str());
    ImGui::PopFont();
}

void Fonts::push(const std::string& fontname) { ImGui::PushFont(mFonts[fontname]); }

void Fonts::pop(void) { ImGui::PopFont(); }

} // namespace GRender