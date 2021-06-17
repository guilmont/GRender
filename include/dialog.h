#pragma once

#include "core.h"

class Dialog
{
public:
    GAPI Dialog(void);
    GAPI ~Dialog(void);

    GAPI void createDialog(uint32_t type, const std::string &title,
                      const std::list<std::string> &ext,
                      void *data = nullptr,
                      void (*callback)(const std::string &, void *) = nullptr);
    GAPI void showDialog(void);

    // RETRIEVE DATA
    GAPI std::string getPath(void) const { return main_path.string(); }

    enum : uint32_t
    {
        OPEN,
        SAVE,
    };

private:
    bool active = false;
    std::string title;

    fs::path main_path;

    std::string currentExt, selected, probable, filename;
    std::list<std::string> lExtension;

    bool (Dialog::*dialog_function)(void);

    // callback
    void *callback_data = nullptr;
    void (*callback)(const std::string &, void *) = nullptr;

    // DISPLAY DIALOGS
    bool openDialog(void);
    bool saveDialog(void);

    bool systemDisplay(std::string url);

    bool existPopup = false;
    bool fileExistsPopup(void);

    friend int inputTextCallback(ImGuiInputTextCallbackData *);
};
