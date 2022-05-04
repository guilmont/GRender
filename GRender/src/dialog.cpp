#include "dialog.h"

#include "internal/dialogImpl.h"

namespace GRender::dialog {

// local pointer for dialog implementaiont
internal::DialogImpl* dialogData = nullptr;

void Create() {
    dialogData = new internal::DialogImpl();
}

void Destroy() {
    delete dialogData;
}

void OpenDirectory(const std::string& title, void (*callback)(const std::filesystem::path&, void*), void* data) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->openDirectory(title, callback, data);
}

void OpenFile(const std::string& title, const std::vector<std::string>& extensions, void (*callback)(const std::filesystem::path&, void*), void* data) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->openFile(title, extensions, callback, data);
}

void SaveFile(const std::string& title, const std::vector<std::string>& extensions, void (*callback)(const std::filesystem::path&, void*), void* data) {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->saveFile(title, extensions, callback, data);
}

void ShowDialog() {
    ASSERT(dialogData, "Dialog module was not created!");
    dialogData->showDialog();
}

} // namespace GRender