#pragma once

#include "core.h"

namespace GRender::dialog {
    // Opens a dialog allowing to seek a directory in the system
    void OpenDirectory(const std::string& title, const std::function<void(const std::filesystem::path&)>& callback);

    // Opens a dialog allowing to seek files in the system
    void OpenFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const std::filesystem::path&)>& callback);

    // Opens a dialog allowing to choose a location and name for a file
    void SaveFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const std::filesystem::path&)>& callback);

    // Internal use to display dialog
    void ShowDialog(void);

    // Sets work default path when opening
    void SetDefaultPath(const fs::path& defaultPath);

} // namespace GRender::dialog