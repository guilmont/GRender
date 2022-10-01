#pragma once

#include "core.h"

#include "events.h"
#include "fonts.h"
#include "dialog.h"
#include "mailbox.h"


namespace GRender {

    class Application {
    public:
        Application(const std::string& name, uint32_t width, uint32_t height, const std::filesystem::path& layout);
        virtual ~Application(void);

        // Flow control
        void closeApp(void);
        void run(void);
        void setAppTitle(const std::string& title);

        // To be implemented by user
        virtual void onUserUpdate(float deltaT) = 0;
        virtual void ImGuiLayer(void) {}
        virtual void ImGuiMenuLayer(void) {}

        void scaleSizes(void); // Toggles the scaling for HIDPI screens

        static void EnableVSync(void);  // Default on
        static void DisableVSync(void);

    private:
        GLFWwindow* window = nullptr;
        float deltaTime = 0.1f;   // This value is going to be uploaded by main loop

        std::string layoutINI;

        // Callback functions to control flow
        friend void winResize_callback(GLFWwindow*, int, int);
        friend void winDrop_callback(GLFWwindow*, int, const char**);
    };

    // Used to manage entry point in Windows, Linux and Mac
    Application* createApplication(int argc, char** argv);
}