#pragma once

#include "core.h"

namespace GRender::dialog {

// Creates dialog implementation
void Create(void);

// Destroys dialog implementation
void Destroy();

// Opens a dialog allowing to seek a directory in the system
void OpenDirectory(const std::string& title, void (*callback)(const std::filesystem::path&, void*), void* data = nullptr);

// Opens a dialog allowing to seek files in the system
void OpenFile(const std::string& title, const std::vector<std::string>& extensions,
                void (*callback)(const std::filesystem::path&, void*), void* data = nullptr);

// Opens a dialog allowing to choose a location and name for a file
void SaveFile(const std::string& title, const std::vector<std::string>& extensions,
                void (*callback)(const std::filesystem::path&, void*), void* data = nullptr);

// Internal use to display dialog
void ShowDialog(void);

} // namespace GRender::dialog