#include "dialog.h"

#include "internal/dialogImpl.h"

namespace GRender::dialog {

// local pointer for dialog implementation
internal::DialogImpl* dialogData = nullptr;

void Create() {
    dialogData = new internal::DialogImpl();
}

void Destroy() {
    delete dialogData;
}

void OpenDirectory(const std::string& title, const std::function<void(const std::filesystem::path&)>& callback) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->openDirectory(title, callback);
}

void OpenFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const std::filesystem::path&)>& callback) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->openFile(title, extensions, callback);
}

void SaveFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const std::filesystem::path&)>& callback) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->saveFile(title, extensions, callback);
}

void ShowDialog() {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->showDialog();
}

void SetDefaultPath(const fs::path& defaultPath) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->setMainPath(defaultPath);
}

} // namespace GRender