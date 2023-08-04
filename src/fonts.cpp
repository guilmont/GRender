#include "fonts.h"

#include "internal/fontsImpl.h"

namespace GRender::fonts {

void Initialize() {
    internal::FontsImpl::Instance();
}

void SetDefault(const std::string& name) {
    internal::FontsImpl::Instance()->setDefault(name);
}

void Swap(const std::string& name1, const std::string& name2) {
    internal::FontsImpl::Instance()->swap(name1, name2);
}

void LoadFont(const std::string& fontname, const std::string& path, float size) {
    internal::FontsImpl::Instance()->loadFont(fontname, path, size);
}

void Text(const std::string& txt, const std::string& fontname) {
    internal::FontsImpl::Instance()->text(txt, fontname);
}

void Push(const std::string& fontname) {
    internal::FontsImpl::Instance()->push(fontname);
}

void Pop(void) {
    internal::FontsImpl::Instance()->pop();
}

} // namespace GRender::fonts