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

struct Window
{
    std::string title = "Renderer";
    glm::vec2 position = { 0.0f, 0.0f }, size = { 10.0f, 10.0f }; // Just whatever
    GLFWwindow *ptr = nullptr;
};

class  Renderer
{
public:
    GAPI Renderer(void) = default;
    GAPI virtual ~Renderer(void);

    GAPI void initialize(const std::string &name, uint32_t width, uint32_t height);
    GAPI void closeApp(void) { glfwSetWindowShouldClose(window.ptr, 1); }
    GAPI void mainLoop(void);

    GAPI virtual void onUserUpdate(float deltaT) = 0;
    GAPI virtual void ImGuiLayer(void) {}
    GAPI virtual void ImGuiMenuLayer(void) {}

    Window window;
    Fonts fonts;
    Mouse mouse;
    Camera camera;
    Keyboard keyboard;
    Shader shader;
    Dialog dialog;

private:
    float deltaTime = 0.1f;
    friend void winResize_callback(GLFWwindow *, int, int);
    friend void winPos_callback(GLFWwindow *, int, int);
    friend void mousePos_callback(GLFWwindow *, double, double);
    friend void mouseScroll_callback(GLFWwindow *, double, double);
    friend void mouseButton_callback(GLFWwindow *, int, int, int);
    friend void keyboard_callback(GLFWwindow *, int, int, int, int);
    friend void winDrop_callback(GLFWwindow*, int, const char**);
};
