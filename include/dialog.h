#pragma once

#include "core.h"

enum class DialogType : uint8_t
{
    OPEN,
    SAVE,
};

class Dialog
{
public:
    GAPI Dialog(void);
    GAPI ~Dialog(void);


    GAPI void createDialog(DialogType type, const std::string &title,
                      const std::list<std::string> &ext,
                      void *data = nullptr,
                      void (*callback)(const std::string &, void *) = nullptr);



    GAPI void showDialog(void);

    // RETRIEVE DATA
    GAPI const fs::path& getPath(void) const { return filePath; }

    // Used to drop files
    struct
    {
        bool handle = false;
        fs::path path;
    } drop;


private:
    bool active = false;
    std::string title;

    fs::path mainPath, filePath, filename;
    
    std::string currentExt;
    std::list<std::string> lExtension;

    bool (Dialog::*dialog_function)(void);

    // callback
    void *callback_data = nullptr;
    void (*callback)(const std::string &, void *) = nullptr;

    // DISPLAY DIALOGS
    bool openDialog(void);
    bool saveDialog(void);

    bool systemDisplay(void);

    bool existPopup = false;
    bool fileExistsPopup(void);

};


