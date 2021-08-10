#include "application.h"

namespace GRender
{
    void winResize_callback(GLFWwindow* window, int width, int height)
    {
        Window& win = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->window;
        win.size = { width, height };
        glad_glViewport(0, 0, width, height);
    } //winResize_callback

    void winPos_callback(GLFWwindow* window, int xpos, int ypos)
    {
        Window& win = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->window;
        win.position = { xpos, ypos };
    }

    void mousePos_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Mouse& mouse = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->mouse;
        mouse.offset = { xpos - mouse.position.x, ypos - mouse.position.y };
        mouse.position = { xpos, ypos };
    } //mousePos_callback

    void mouseButton_callback(GLFWwindow* window, int button, int action, int mods)
    {
        Mouse& mouse = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->mouse;
        mouse.set(button, action);
    } // mouseButton_callback

    void mouseScroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Mouse& mouse = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->mouse;
        mouse.wheel = { float(xoffset), float(yoffset) };

    } // mouseScroll_callback

    void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Keyboard& keyboard = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->keyboard;
        keyboard.set(key, action);
    } // keyboard_callback

    void winDrop_callback(GLFWwindow* window, int nDrops, const char** dropPath)
    {
        Dialog& diag = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))->dialog;
        if (diag.active)
            diag.drop = { true, fs::path{dropPath[0]} };
    }

    /*****************************************************************************/
    /*****************************************************************************/

    Application::~Application(void)
    {
        texture.reset();
        shader.deletePrograms();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

#ifdef BUILD_IMPLOT
        ImPlot::DestroyContext();
#endif

        ImGui::DestroyContext();

        glfwTerminate();
    } // destructor

    void Application::initialize(const std::string& name, uint32_t width, uint32_t height, const char *layout)
    {

        window.title = name;
        window.size = { width * DPI_FACTOR,height * DPI_FACTOR };
        window.position = 0.1f * window.size;

        // Setup window
        glfwSetErrorCallback([](int error, const char* description) -> void {
            pout("ERROR (glfw): ", error, " :: ", description);
            exit(0);
            });

        if (!glfwInit())
        {
            pout("ERROR (glfw): Couldn't start glfw!!!");
            exit(-1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

#ifdef __APPLE__  // yeah, apple sucks
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Generating resizable window
        window.ptr = glfwCreateWindow(static_cast<int>(window.size.x), static_cast<int>(window.size.y), name.c_str(), NULL, NULL);
        if (window.ptr == NULL)
        {
            pout("ERROR: Failed to create GLFW window!!");
            glfwTerminate();
            exit(-1);
        }

        glfwSetInputMode(window.ptr, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwMakeContextCurrent(window.ptr);
        glfwSwapInterval(1); // synchronize with screen updates

        // Initialize OPENGL loader
        if (gladLoadGL() == 0)
        {
            pout("ERROR (glad): Failed to initialize OpenGL loader!!!");
            exit(-1);
        }


        // Turning on error events only in debug mode
#ifdef _DEBUG
        glad_glEnable(GL_DEBUG_OUTPUT);
        glad_glDebugMessageCallback(glErrorCallback, nullptr);
#endif

        // Setup viewport -> opengl is going transform final coordinates into this range
        glad_glViewport(0, 0, static_cast<int>(window.size.x), static_cast<int>(window.size.y));

        ///////////////////////////////////////////////////////////////////////////
        // HANDLING WINDOW PROPERTIES
        glfwSetWindowPos(window.ptr, static_cast<int>(window.position.x), static_cast<int>(window.position.y));

        glfwSetWindowUserPointer(window.ptr, this);
        glfwSetWindowPosCallback(window.ptr, winPos_callback);
        glfwSetKeyCallback(window.ptr, keyboard_callback);
        glfwSetCursorPosCallback(window.ptr, mousePos_callback);
        glfwSetMouseButtonCallback(window.ptr, mouseButton_callback);
        glfwSetScrollCallback(window.ptr, mouseScroll_callback);
        glfwSetWindowSizeCallback(window.ptr, winResize_callback);
        glfwSetDropCallback(window.ptr, winDrop_callback);


        ///////////////////////////////////////////////////////////////////////////
        // SETUP DEAR IMGApplication/IMPLOT CONTEXT

        // Setup Platform/Application bindings
        ImGui::CreateContext();

#ifdef BUILD_IMPLOT
        ImPlot::CreateContext();
#endif

        ImGui::StyleColorsClassic();

        // Floating windows off main windows
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = layout;

        // Rescaling all sizes to account for HIDPI screens
        ImGuiStyle &style = ImGui::GetStyle();
        style.ScaleAllSizes(DPI_FACTOR);

        ImGui_ImplGlfw_InitForOpenGL(window.ptr, true);
        ImGui_ImplOpenGL3_Init("#version 410 core"); // Mac supports only up to 410

        fs::path assets(ASSETS);

        // Setup fonts
        fonts.loadFont("regular", (assets / "Open_Sans/OpenSans-Regular.ttf").string(), 18.0 * DPI_FACTOR);
        fonts.loadFont("bold", (assets / "Open_Sans/OpenSans-Bold.ttf").string(), 18.0 * DPI_FACTOR);
        fonts.setDefault("regular");

    } 

    void Application::run(void)
    {
        glfwSetTime(0);
        double t0 = glfwGetTime();

        // resetProgram(data);
        while (!glfwWindowShouldClose(window.ptr))
        {
            // reset events
            keyboard.clear();
            mouse.clear();

            // Get new events
            glfwPollEvents();

            // Updating application
            onUserUpdate(deltaTime);

            ///////////////////////////////////////////////////
            // IMGUI /////////////////////////////////////////

            // Setup a new frame for imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
            window_flags |= ImGuiWindowFlags_MenuBar;
            window_flags |= ImGuiWindowFlags_NoDocking;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            window_flags |= ImGuiWindowFlags_NoNavFocus;

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);


            static bool p_open = true;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace", &p_open, window_flags);
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

            if (ImGui::BeginMenuBar())
            {
                ImGuiMenuLayer();
                ImGui::EndMenuBar();
            }

            // Here goes the implementation for user interface
            ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0, 0.0, 0.0, 1.0 }); // solid background
            
            // displaying dialog if active
            dialog.showDialog();

            ImGuiLayer();
            ImGui::PopStyleColor();

            // Ending dockspace
            ImGui::End();

            // Render ImGui // It calls endFrame automatically
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Allows rendering of external floating windows
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);

            ///////////////////////////////////////////////////

            // Swap secondary buffer to screen
            glfwSwapBuffers(window.ptr);

            // Calculating elapsed time for smooth controls
            double tf = glfwGetTime();
            deltaTime = float(tf - t0);
            t0 = tf;

        } // main-loop

    }

}