#include "dialog.h"

#include "internal/dialogImpl.h"

namespace GRender::dialog {

void OpenDirectory(const std::string& title, const std::function<void(const std::filesystem::path&)>& callback) {
    internal::DialogImpl::Instance()->openDirectory(title, callback);
}

void OpenFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const std::filesystem::path&)>& callback) {
    internal::DialogImpl::Instance()->openFile(title, extensions, callback);
}

void SaveFile(const std::string& title, const std::vector<std::string>& extensions, const std::function<void(const std::filesystem::path&)>& callback) {
    internal::DialogImpl::Instance()->saveFile(title, extensions, callback);
}

void ShowDialog() {
    internal::DialogImpl::Instance()->showDialog();
}

void SetDefaultPath(const fs::path& defaultPath) {
    internal::DialogImpl::Instance()->setMainPath(defaultPath);
}

} // namespace GRender