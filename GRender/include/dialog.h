#pragma once

#include "core.h"

namespace GRender {

class Dialog {
public:
    Dialog(void);
    ~Dialog(void) = default;

    void openDirectory(const std::string& title, void (*callback)(const std::filesystem::path&, void*), void* data);

    void openFile(const std::string& title, const std::vector<std::string>& extensions,
                  void (*callback)(const std::filesystem::path&, void*), void* data);

    void saveFile(const std::string& title, const std::vector<std::string>& extensions,
                  void (*callback)(const std::filesystem::path&, void*), void* data);

    void showDialog(void);

private:
    void (Dialog::*internalShow)(void) = nullptr; // points to one of the following functions
    
    void showOpenDirectory(void);
    void showOpenFile(void);
    void showSaveFile(void);

private:
    bool mActive = false;
    bool mExistsPopup = false;
    
    std::string mTitle;
    std::vector<std::string> mExtensions;
    std::string mCurrentExt = "";

    void (*mCallback)(const std::filesystem::path&, void*) = nullptr;
    void* mCallbackData = nullptr;

    std::string filename = "";
    std::filesystem::path mainpath;
    std::vector<std::filesystem::path> availablePaths;

    void updateAvailablePaths(void);
    bool systemDisplay(void);
    void fileExistsPopup(void);
    
private:
    const glm::vec2 mSize = { 720.0f, 480.0f };

};

} // namespace GRender::dialog