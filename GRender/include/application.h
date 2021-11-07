#pragma once

#include "core.h"

// local
#include "quad.h"
#include "shader.h"
#include "framebuffer.h"
#include "texture.h"
#include "events.h"
#include "camera.h"
#include "fonts.h"
#include "dialog.h"
#include "mailbox.h"


namespace GRender {

    struct Window
    {
        std::string title = "Application";
        glm::vec2 position = { 0.0f, 0.0f }, size = { 10.0f, 10.0f }; // Just whatever
        GLFWwindow* ptr = nullptr;
    };

    class Application
    {
    public:
        Application(void) = default;
        virtual ~Application(void);

        // Flow control
        void initialize(const std::string& name, uint32_t width, uint32_t height, const fs::path& layout);
        void closeApp(void) { glfwSetWindowShouldClose(window.ptr, 1); }
        void run(void);

        // To be implemented by user
        virtual void onUserUpdate(float deltaT) = 0;
        virtual void ImGuiLayer(void) {}
        virtual void ImGuiMenuLayer(void) {}

        // Basic utilities
        Window window;
        Fonts fonts;
        Mouse mouse;
        Keyboard keyboard;
        Dialog dialog;
        Mailbox mailbox;

    protected:
        void scaleSizes(void); // Used to rescales sizes for HIDPI screens

    private:
        float deltaTime = 0.1f;   // This value is going to be uploaded by main loop

        std::string layoutINI;

        // Callback functions to control flow
        friend void winResize_callback(GLFWwindow*, int, int);
        friend void winPos_callback(GLFWwindow*, int, int);
        friend void mousePos_callback(GLFWwindow*, double, double);
        friend void mouseScroll_callback(GLFWwindow*, double, double);
        friend void mouseButton_callback(GLFWwindow*, int, int, int);
        friend void keyboard_callback(GLFWwindow*, int, int, int, int);
        friend void winDrop_callback(GLFWwindow*, int, const char**);
    };

}