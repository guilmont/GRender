#pragma once

#include <list>

#include "core.h"

enum class GDialog : const uint8_t
{
    OPEN,
    OPEN_DIRECTORY,
    SAVE,
};


namespace GRender {

class Dialog {

public:
    Dialog(void);
    ~Dialog(void);


    void createDialog(GDialog type, const std::string& title,
        const std::list<std::string>& ext,
        void* data = nullptr,
        void (*callback)(const std::filesystem::path&, void*) = nullptr);

    void showDialog(void);

    // RETRIEVE DATA
    const std::filesystem::path& getPath(void) const { return filePath; }

    // Used to drop files
    struct
    {
        bool handle = false;
        std::filesystem::path path;
    } drop;


private:
    bool active = false;
    GDialog myType;

    std::string title;
    glm::vec2 size = { 720.0f, 480.0f };

    std::filesystem::path mainPath, filePath, filename;

    std::string currentExt;
    std::list<std::string> lExtension;

    bool (Dialog::* dialog_function)(void);

    // callback
    void* callback_data = nullptr;
    void (*callback)(const std::filesystem::path&, void*) = nullptr;

    // DISPLAY DIALOGS
    bool openDialog(void);
    bool openDirectory(void);
    bool saveDialog(void);

    bool systemDisplay(void);

    bool existPopup = false;
    bool fileExistsPopup(void);


private:
    friend void winDrop_callback(GLFWwindow*, int, const char**); // so we can drop stuff only when dialog windows are openned
};

} // namespace GRender