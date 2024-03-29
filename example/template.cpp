#include <random>
#include <thread>
#include <algorithm>

#include "GRender/application.h"
#include "GRender/entryPoint.h"

#include "GRender/camera.h"
#include "GRender/computeShader.h"
#include "GRender/interactiveImage.h"
#include "GRender/orbitalCamera.h"
#include "GRender/quad.h"
#include "GRender/table.h"
#include "GRender/utils.h"
#include "GRender/viewport.h"

#include "GRender/objects/cube.h"
#include "GRender/objects/sphere.h"
#include "GRender/objects/cylinder.h"

#include "polymer.h"

namespace fs = std::filesystem;

class Sandbox : public GRender::Application
{
public:
    Sandbox(const std::string& title = "Sandbox");
    ~Sandbox(void) = default;

    void onUserUpdate(float deltaTime) override;
    void ImGuiLayer(void) override;
    void ImGuiMenuLayer(void) override;

private:
    bool
        view_specs = false,
        view_imguidemo = false,
        view_messages = true,
        viewport_hover = false,
        progressCancel = false,
        timerCancel = false;

#ifdef BUILD_IMPLOT
    bool view_implotdemo = false;
#endif

    GRender::ComputeShader compShader;
    GRender::Table<GRender::Texture> texture;

    bool useOrbitalCamera = true;
    GRender::Camera camera;
    GRender::OrbitalCamera orbital;

    GRender::Quad quad;
    polymer::Polymer poly;

    GRender::Cube cube;
    GRender::Sphere sphere;
    GRender::Cylinder cylinder;

    GRender::Viewport view;
    GRender::InteractiveImage interact;

    glm::vec3 bgColor = { 0.3f, 0.3f, 0.3f };

};

GRender::Application* GRender::createApplication(int argc, char** argv) {
    namespace fs = std::filesystem;
    // Processing inputs to use current path
    const fs::path pwd = fs::current_path();
    GRender::dialog::SetDefaultPath(pwd);

    // Setup program to use install path as reference
    const fs::path projPath = fs::canonical(argv[0]).parent_path().parent_path();
    fs::current_path(projPath);

    INFO("Project path: " + fs::current_path().string());

    if (argc == 1) {
        return new Sandbox;
    }
    else {
        return new Sandbox((pwd / fs::path{argv[1]}).string());
    }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void testProgress(bool* cancel, float* progress) {
    *cancel = false;
    while (*progress < 1.0f && (*cancel == false)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        *progress += 0.05f;
    }
    // automatically detects when progress >= 1.0f
}

static void testTimer(bool* cancel, GRender::Timer* timer) {
    *cancel = false;
    uint32_t ct = 0;
    while (++ct < 10 && (*cancel == false)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    timer->stop();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Sandbox::Sandbox(const std::string& title) : Application(title, 1200, 800, "assets/layout.ini") {
    compShader = GRender::ComputeShader(fs::path{ "assets/compute.cmp.glsl" });

    GRender::texture::Specification defSpec;
    view = GRender::Viewport({ 1200, 800 }, { defSpec }, true);

    camera = GRender::Camera({ 0.0f, 0.0f, 25.0f });
    camera.open();

    orbital = GRender::OrbitalCamera({0.0f, 0.0f, 0.0f}, 25.0f);
    orbital.open();

    quad = GRender::Quad(1);
    cube = GRender::Cube(6 * 11 * 11);
    sphere = GRender::Sphere(1);
    cylinder = GRender::Cylinder(1);

    using namespace GRender::texture;
    Specification spec;
    spec.wrap.x = Wrap::MIRRORED;
    spec.wrap.y = Wrap::REPEAT;
    texture.insert("space", GRender::utils::createTextureFromRGBAFile("assets/space.jpg", spec));
    texture.insert("earth", GRender::utils::createTextureFromRGBAFile("assets/earth.jpg", spec));

    spec.fmt = Format::RGBA8;
    spec.wrap.x = Wrap::BORDER;
    spec.wrap.y = Wrap::BORDER;
    texture.emplace("image", glm::uvec2{ 800, 640 }, spec);

    glEnable(GL_DEPTH_TEST);
    poly = polymer::Polymer(128, 1.0f);

    interact = GRender::InteractiveImage("assets/earth.jpg");
}

void Sandbox::onUserUpdate(float deltaTime) {
    using namespace GRender;

    bool ctrl = keyboard::IsDown(Key::LEFT_CONTROL) || keyboard::IsDown(Key::RIGHT_CONTROL);

    if (ctrl && keyboard::IsPressed('H')) { view_specs = true; }

    if (ctrl && keyboard::IsPressed('I')) { view_imguidemo = true; }

    if (keyboard::IsPressed('I')) {
        static bool toggle = false;
        toggle = !toggle;
        toggle ? interact.open() : interact.close();
    }

    if (ctrl && keyboard::IsPressed('O')) {
        auto callback = [](const fs::path& path) -> void { mailbox::CreateInfo("Selected file: " + path.string()); };
        dialog::OpenFile("Open file...", { "txt", "json" }, callback);
    }

    if (ctrl && keyboard::IsPressed('S')) {
        auto callback = [](const fs::path& path) -> void { mailbox::CreateInfo("Save file: " + path.string()); };
        dialog::SaveFile("Save file...", {"txt", "json"}, callback);
    }

    if (ctrl && keyboard::IsPressed('D')) {
        auto callback = [](const fs::path& path) -> void { mailbox::CreateInfo("Open directory: " + path.string()); };
        dialog::OpenDirectory("Open directory...", callback);
    }

#ifdef BUILD_IMPLOT
    if (ctrl && keyboard::IsPressed('P'))
        view_implotdemo = true;
#endif

    if (ctrl && keyboard::IsPressed('M'))
        view_messages = true;

    ///////////////////////////////////////////////////////////////////////////
    // Camera
    if (view.hovered() && !ctrl) { // !ctrl not to interfere with commands used above
        if (useOrbitalCamera) { orbital.controls(deltaTime); }
        else                  { camera.controls(deltaTime);  }
    }

    ///////////////////////////////////////////////////////////////////////////

    const glm::mat4& viewMatrix = useOrbitalCamera ? orbital.getViewMatrix() : camera.getViewMatrix();

    view.bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);

    // QUAD ///////////////////////////////////////////////
    static float tt = 0;
    tt += deltaTime;
    auto sz = texture["space"].size();

    quad::Specification spec;
    spec.position = { 6.0f + sin(tt), 7.0f, 0.0f};
    spec.size = 1.5f * glm::vec2{ float(sz.x) / float(sz.y), 1.0f };
    spec.texCoord = glm::vec4{ 0.0f, 0.0f, 2.0f, 2.0f };
    spec.texture = &texture["space"];

    quad.submit(spec);
    quad.draw(viewMatrix);

    ///////////////////////////////////////////////////////
    // POLYMER ////////////////////////////////////////////
    poly.draw(viewMatrix);

    // CUBE ///////////////////////////////////////////////
    object::Specification obj;
    obj.position = {cos(tt), 7.0f + sin(tt), 0.0f };
    obj.rotation = { 0.0f, tt, 0.2f * tt };
    obj.scale.x = 1.0f + 0.7f * cos(tt);
    obj.texture = &texture["earth"];
    cube.submit(obj);

    // SPHERE OF CUBES ////////////////////////////////////
    float osc = 4.0f * (1.0f + 0.5f * cos(tt));
    glm::vec3 com = { -7.0f, 6.0f, 0.0f };

    obj.scale = glm::vec3{0.2f};
    obj.texture = nullptr;

    const size_t N = 7;
    const size_t NY = N + 2 * (N >> 1);
    float dTheta = glm::pi<float>() / float(NY - 1);

    obj.position = com + osc * glm::vec3{0.0f, 0.5f, 0.0f};
    obj.color = {1.0f, 1.0f, 0.0f, 1.0f};
    cube.submit(obj);

    for (size_t k = 0; k < NY; k++) {
        float theta = k * dTheta;
        size_t NX = k < (N + (N>>1)) ? std::min(2*k+1, N) : 2 * (NY - k - 1) + 1;

        float dPhi = glm::half_pi<float>() / float(NX - 1);

        for (size_t l = 0; l < 4*(NX-1); l++) {
            float phi = l * dPhi;

            obj.position = com + osc * 0.5f * glm::vec3{cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta)};
            obj.color = {1.0f - phi / glm::two_pi<float>(), 1.0f - theta / glm::pi<float>(), 0.0f, 1.0f};
            cube.submit(obj);
        }
    }

    obj.position = com + osc * glm::vec3{0.0f, -0.5f, 0.0f};
    obj.color = {0.0f, 0.0f, 0.0f, 1.0f};
    cube.submit(obj);
    cube.draw(viewMatrix);

    // SPHERE ////////////////////////////////////////////////
    object::Specification obj2;
    obj2.position = { 7.0f, -5.0f, 0.0f };
    obj2.rotation = { 0.0f, -0.5f*tt, 0.0f };
    obj2.scale = glm::vec3{ 4.0f };
    obj2.texture = &texture["earth"];
    sphere.submit(obj2);
    sphere.draw(viewMatrix);

    // CYLINDER ///////////////////////////////////////////
    object::Specification obj3;
    obj3.position = {-8.0f, -6.0f, 0.0f};
    obj3.scale = glm::vec3(2.0f);
    obj3.rotation = { tt, 0.5f*cos(tt), 0.5f * 3.1415f };
    obj3.texture = &texture["space"];
    cylinder.submit(obj3);
    cylinder.draw(viewMatrix);

    view.unbind();

    ///////////////////////////////////////////////////////
    // COMPUTE SHADER /////////////////////////////////////

    const glm::uvec2  dimensions = texture["image"].size();
    const uint32_t numGroupsX = dimensions.x / 32;
    const uint32_t numGroupsY = dimensions.y / 32;

    compShader.bind();
    compShader.setUniform("time", tt);
    compShader.setTexture(texture["image"]);
    compShader.dispatch(numGroupsX, numGroupsY);
}

void Sandbox::ImGuiLayer(void) {
    using namespace GRender;

    utils::PerformanceDisplay(view_specs);
    utils::ViewWidgetsDemo(view_imguidemo);

    if (useOrbitalCamera) { orbital.display(); }
    else                  { camera.display(); }


#ifdef BUILD_IMPLOT
    if (view_implotdemo)
        ImPlot::ShowDemoWindow(&view_implotdemo);
#endif

    if (view_messages) {
        ImGui::Begin("Messages", &view_messages);
        if (ImGui::Button("Info")) { mailbox::CreateInfo("Information"); }
        ImGui::SameLine();
        if (ImGui::Button("Warn")) { mailbox::CreateWarn("Warning"); }
        ImGui::SameLine();
        if (ImGui::Button("Error")) { mailbox::CreateError("There was an error!"); }
        ImGui::SameLine();
        if (ImGui::Button("Progress")) {
            GRender::Progress* prog = mailbox::CreateProgress("Running...", [this]() { progressCancel = true; });
            std::thread thr(testProgress, &progressCancel, &prog->progress);
            thr.detach();
        }
        ImGui::SameLine();
        if (ImGui::Button("Timer")) {
            GRender::Timer* timer = mailbox::CreateTimer("Timing stuff...", [this]() { timerCancel = true; });
            std::thread thr(testTimer, &timerCancel, timer);
            thr.detach();
        }
        ImGui::End();
    }

    //////////////////////////////////////////////////////////////////////////////
    // Polymer control
    ImGui::Begin("Polymer");
    const float split = 0.3f;
    const float width = 0.43f;

    uint32_t numBeads = poly.numBeads();
    if (utils::Drag<uint32_t>("NumBeads:", numBeads, split, width, 1.0f, 1, 8192) && numBeads > 0) {
        float kuhn = poly.kuhn();
        float radius = poly.radius();
        poly = polymer::Polymer(numBeads, kuhn);
        poly.radius() = radius;
    }

    utils::Drag("Radius:", poly.radius(), split, width, 0.1f, 0.1f, 5.0f, "%.2f");

    utils::RGB_Edit("Spheres:", poly.sphereColor(), split);
    utils::RGB_Edit("Connections:", poly.cylinderColor(), split);
    utils::RGB_Edit("Background:", bgColor, split);

    utils::Checkbox("Orbital camera:", useOrbitalCamera, split);
    ImGui::End();

    // Show viewport
    view.display("Viewport");

    // Update camera aspect ratio in case viewport changed
    glm::vec2 vsz = view.size();
    orbital.aspectRatio() = vsz.x / vsz.y;
    camera.aspectRatio() = vsz.x / vsz.y;

    ///////////////////////////////////////////////////////
    // Small display for the compute shader
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
    ImGui::Begin("Compute result");
    ImVec2 port = ImGui::GetContentRegionAvail();
    ImGui::Image((void*)(uintptr_t)texture["image"].id(), port, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::End();
    ImGui::PopStyleVar();

    ///////////////////////////////////////////////////////
    // Interactive image
    interact.display();
}

void Sandbox::ImGuiMenuLayer(void) {
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open...", "Ctrl+O")) {
            auto callback = [](const fs::path& path) -> void { GRender::mailbox::CreateInfo("Selected file: " + path.string()); };
            GRender::dialog::OpenFile("Open file...", { "txt", "json" }, callback);
        }

        if (ImGui::MenuItem("Save...", "Ctrl+S")) {
            auto callback = [](const fs::path& path) -> void { GRender::mailbox::CreateInfo("Save file: " + path.string()); };
            GRender::dialog::SaveFile("Save file...", {"txt", "json"}, callback);
        }

        if (ImGui::MenuItem("Open directory...", "Ctrl+D")) {
            auto callback = [](const fs::path& path) -> void { GRender::mailbox::CreateInfo("Open directory: " + path.string()); };
            GRender::dialog::OpenDirectory("Open directory...", callback);
        }

        if (ImGui::MenuItem("Exit"))
            closeApp();

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::BeginMenu("Camera controls")) {
            if (ImGui::MenuItem("Free")) { useOrbitalCamera = false; camera.open(); }
            if (ImGui::MenuItem("Orbital")) { useOrbitalCamera = true; orbital.open(); }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Interactive image", "I")) {
            interact.open();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("About"))
    {
        if (ImGui::MenuItem("Specs", "Ctrl+H"))
            view_specs = true;

        if (ImGui::MenuItem("ImGui Demo", "Ctrl+D"))
            view_imguidemo = true;

#ifdef BUILD_IMPLOT
        if (ImGui::MenuItem("ImPlot Demo", "Ctrl+P"))
            view_implotdemo = true;
#endif

        if (ImGui::MenuItem("Messages Demo", "Ctrl+M"))
            view_messages = true;

        if (ImGui::MenuItem("View mailbox"))
            GRender::mailbox::Open();

        ImGui::EndMenu();
    }
}
