#pragma once

#include "core.h"


class  Fonts
{
public:
    void loadFont(const std::string &fontname, const std::string &path, float size);
    void setDefault(const std::string &name);

    void text(const std::string &txt, const std::string &type);

    void push(const std::string &fontname);
    void pop(void);

private:
    std::string assets;
    std::unordered_map<std::string, ImFont *> mFonts;
};
