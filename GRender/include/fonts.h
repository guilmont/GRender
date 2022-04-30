#pragma once

#include <unordered_map>

#include "core.h"

namespace GRender::fonts {

static std::unordered_map<std::string, ImFont *> mFonts;

void LoadDefaultFonts(void);
void SetDefault(const std::string &name);

void LoadFont(const std::string &fontname, const std::string &path, float size);

// Used for DPI switching
void Swap(const std::string& name1, const std::string& name2);

void Text(const std::string &txt, const std::string &type);
void Push(const std::string &fontname);
void Pop(void);


} // namespace GRender