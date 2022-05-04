#include "fonts.h"

#include "internal/fontsImpl.h"

namespace GRender::fonts {

internal::FontsImpl* fontsData = nullptr;

void Create() {
    fontsData = new internal::FontsImpl();
}

void Destroy() {
    delete fontsData;
}

void SetDefault(const std::string& name) {
    ASSERT(fontsData, "Font module was not created!");
    fontsData->setDefault(name);
}

void Swap(const std::string& name1, const std::string& name2) {
    ASSERT(fontsData, "Font module was not created!");
    fontsData->swap(name1, name2);
}

void LoadFont(const std::string& fontname, const std::string& path, float size) {
    ASSERT(fontsData, "Font module was not created!");
    fontsData->loadFont(fontname, path, size);
}

void Text(const std::string& txt, const std::string& fontname) {
    ASSERT(fontsData, "Font module was not created!");
    fontsData->text(txt, fontname);
}

void Push(const std::string& fontname) {
    ASSERT(fontsData, "Font module was not created!");
    fontsData->push(fontname);
}

void Pop(void) {
    ASSERT(fontsData, "Font module was not created!");
    fontsData->pop();
}

} // namespace GRender::fonts