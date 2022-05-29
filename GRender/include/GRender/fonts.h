#pragma once

#include <string>

namespace GRender::fonts {

void Create(void);
void Destroy(void);

void SetDefault(const std::string &name);

void LoadFont(const std::string &fontname, const std::string &path, float size);

// Used for DPI switching
void Swap(const std::string& name1, const std::string& name2);

void Text(const std::string &txt, const std::string &type);
void Push(const std::string &fontname);
void Pop(void);

} // namespace GRender