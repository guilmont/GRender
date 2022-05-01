#include "application.h"

namespace GRender {
    
void winResize_callback(GLFWwindow* window, int width, int height) {
    glad_glViewport(0, 0, width, height);
}
   
/*****************************************************************************/
/*****************************************************************************/


Application::Application(const std::string& name, uint32_t width, uint32_t height,
                         const std::filesystem::path& layout) {

    namespace fs = std::filesystem;

    // Hack to corrent for HiDPI screens
    width = static_cast<uint32_t>(DPI_FACTOR * width);
    height = static_cast<uint32_t>(DPI_FACTOR * height);

    // Setup window
    glfwSetErrorCallback([](int error, const char* description) -> void {
        ASSERT(false, "(glfw) -> " + std::to_string(error) + " :: " + std::string(description));
        });

    ASSERT(glfwInit(),"(glfw) -> Couldn't start glfw!!!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

#ifdef __APPLE__  // yeah, apple sucks
    const char* glsl_version = "#version 410 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
    const char* glsl_version = "#version 450 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Generating resizable window
    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), NULL, NULL);
    ASSERT(window, "(glfw) -> Failed to create GLFW window!!");

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // synchronize with screen updates

    // Initialize OPENGL loader
    ASSERT(gladLoadGL() != 0,"(glad) -> Failed to initialize OpenGL loader!!!");

    // Turning on error events only in debug mode
#ifdef _DEBUG
    glad_glEnable(GL_DEBUG_OUTPUT);
    glad_glDebugMessageCallback(glErrorCallback, nullptr);
#endif

    // Setup viewport -> opengl is going transform final coordinates into this range
    glad_glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));

    ///////////////////////////////////////////////////////////////////////////
    // HANDLING WINDOW PROPERTIES
    glfwSetWindowPos(window, static_cast<int>(0.1f * width), static_cast<int>(0.1f*height));

    glfwSetWindowUserPointer(window, this);

    ///////////////////////////////////////////////////////////////////////////
    // SETUP DEAR IMGApplication/IMPLOT CONTEXT

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

    // Setup path to layout
    layoutINI = layout.string();
    io.IniFilename = layoutINI.c_str();
        
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup fonts
    fonts::LoadDefaultFonts();

} 

Application::~Application(void) {
    mailbox::Destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

#ifdef BUILD_IMPLOT
    ImPlot::DestroyContext();
#endif

    ImGui::DestroyContext();
    glfwTerminate();
}

void Application::closeApp(void) {
    glfwSetWindowShouldClose(window, 1);
}

void Application::scaleSizes() {
    // Rescaling all sizes to account for HIDPI screens
    ImGuiStyle& style = ImGui::GetStyle();

    if (DPI_FACTOR == 2) {
        style.ScaleAllSizes(0.5f);
        DPI_FACTOR = 1.0f;
    }
    else {
        style.ScaleAllSizes(2.0f);
        DPI_FACTOR = 2.0f;
    }

    fonts::Swap("regular", "regularDPI");
    fonts::Swap("bold",    "boldDPI");
    fonts::Swap("italic",  "italicDPI");
    fonts::SetDefault("regular");
}

void Application::run(void) {
    // Setup timer
    glfwSetTime(0);
    double t0 = glfwGetTime();

    // Starting run time loop
    while (!glfwWindowShouldClose(window)) {
        // Get new events
        glfwPollEvents();

        // Setup a new frame for imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Updating application
        onUserUpdate(deltaTime);

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

        if (ImGui::BeginMenuBar()) {
            ImGuiMenuLayer();
            ImGui::EndMenuBar();
        }

        // Here goes the implementation for user interface
        ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0, 0.0, 0.0, 1.0 }); // solid background
            
        // displaying dialog if active
        dialog::ShowDialog();
        mailbox::ShowMessages();

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
        glfwSwapBuffers(window);

        // Calculating elapsed time for smooth controls
        double tf = glfwGetTime();
        deltaTime = float(tf - t0);
        t0 = tf;
    }
}

} // namespace GRender