#pragma once

#include "core.h"

enum class GDialog: const uint8_t
{
    OPEN,
    SAVE,
};


namespace GRender
{
    class Dialog
    {
    public:
       Dialog(void);
        ~Dialog(void);


       void createDialog(GDialog type, const std::string& title,
            const std::list<std::string>& ext,
            void* data = nullptr,
            void (*callback)(const std::string&, void*) = nullptr);



       void showDialog(void);

        // RETRIEVE DATA
       const fs::path& getPath(void) const { return filePath; }

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

        bool (Dialog::* dialog_function)(void);

        // callback
        void* callback_data = nullptr;
        void (*callback)(const std::string&, void*) = nullptr;

        // DISPLAY DIALOGS
        bool openDialog(void);
        bool saveDialog(void);

        bool systemDisplay(void);

        bool existPopup = false;
        bool fileExistsPopup(void);


    private:
        friend void winDrop_callback(GLFWwindow*, int, const char**); // so we can drop stuff only when dialog windows are openned
    };

}