#include "GRender/application.h"
#include "GRender/entryPoint.h"

#include "polymer.h"

#include <random>
#include <thread>

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
		view_messages = false,
		viewport_hover = false;

#ifdef BUILD_IMPLOT
	bool view_implotdemo = false;
#endif

	GRender::Camera camera;
	GRender::Framebuffer fbuffer;
	GRender::Table<GRender::Shader> shader;

	polymer::Polymer poly;

	glm::vec3 bgColor = { 0.3f, 0.3f, 0.3f };

};

GRender::Application* GRender::createApplication(int argc, char** argv) {
	// Setup program to use executable path as reference
	namespace fs = std::filesystem;

	fs::path exe = fs::path{ argv[0] }.parent_path();
	if (fs::exists(exe))
		fs::current_path(exe);

	if (argc == 1)
		return new Sandbox;
	else
		return new Sandbox(argv[1]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static bool cancel = false;

static void cancelFunction(void*) {
	// this will cause confusion is progress and timer are running in parallel, but it is just a quick demo
	cancel = true; 
};

static void testProgress(float* progress) {
	cancel = false;
	while (*progress < 1.0f && !cancel) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		*progress += 0.05f;
	}
	// automatically detects when progress >= 1.0f
};

static void testTimer(GRender::Timer* timer) {
	cancel = false;
	uint32_t ct = 0;
	while (++ct < 10 && !cancel) {
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	timer->stop();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Sandbox::Sandbox(const std::string& title) : Application(title, 1200, 800, "../assets/layout.ini") {
	fs::path assets("../assets");
	shader.insert("polyBlobs",       { assets / "polyBlobs.vtx.glsl",       assets / "polyShader.frag.glsl" });
	shader.insert("polyConnections", { assets / "polyConnections.vtx.glsl", assets / "polyShader.frag.glsl" });
	
	fbuffer = GRender::Framebuffer(1200, 800);
	camera = GRender::Camera({0.0f, 0.0f, -10.0f});
	camera.open();

	glEnable(GL_DEPTH_TEST);
	poly = polymer::Polymer(128, 1.0f);
}

void Sandbox::onUserUpdate(float deltaTime) {
	using namespace GRender;

	bool ctrl = keyboard::isDown(Key::LEFT_CONTROL) || keyboard::isDown(Key::RIGHT_CONTROL);

	if (ctrl && keyboard::isPressed('H'))
		view_specs = true;

	if (ctrl && keyboard::isPressed('I'))
		view_imguidemo = true;

	if (ctrl && keyboard::isPressed('O')) {
		auto callback = [](const fs::path& path, void*) -> void { mailbox::CreateInfo("Selected file: " + path.string()); };
		dialog::OpenFile("Open file...", { "txt", "json" }, callback);
	}

	if (ctrl && keyboard::isPressed('S')) {
		auto callback = [](const fs::path& path, void*) -> void { mailbox::CreateInfo("Save file: " + path.string()); };
		dialog::SaveFile("Save file...", {"txt", "json"}, callback);
	}

	if (ctrl && keyboard::isPressed('D')) {
		auto callback = [](const fs::path& path, void*) -> void {
			mailbox::CreateInfo("Open directory: " + path.string());
		};
		dialog::OpenDirectory("Open directory...", callback);
	}

#ifdef BUILD_IMPLOT
	if (ctrl && keyboard::isPressed('P'))
		view_implotdemo = true;
#endif

	if (ctrl && keyboard::isPressed('M'))
		view_messages = true;

	///////////////////////////////////////////////////////////////////////////
	// Camera
	if (viewport_hover && !ctrl)  // !ctrl not to interfere with commands used above
		camera.controls(deltaTime);
	
	///////////////////////////////////////////////////////////////////////////

	fbuffer.bind();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);

	// Drawing polymer
	float radius = poly.getRadius();
	GRender::Shader& sCnt = shader["polyConnections"].bind();
	sCnt.setMatrix4f("u_transform", glm::value_ptr(camera.getViewMatrix()));
	sCnt.setFloat("u_radius", 0.5f*radius);
	poly.submitConnections();
	
	shader["polyBlobs"].bind();
	shader["polyBlobs"].setMatrix4f("u_transform", glm::value_ptr(camera.getViewMatrix()));
	shader["polyBlobs"].setFloat("u_radius", radius);
	poly.submitBlobs();

	fbuffer.unbind();
}

void Sandbox::ImGuiLayer(void) {
	using namespace GRender;

	if (view_specs) {
		ImGui::Begin("Specs", &view_specs);
		ImGui::Text("FT: %.3f ms", 1000.0 * double(ImGui::GetIO().DeltaTime));
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::Text("Vendor: %s", glad_glGetString(GL_VENDOR));
		ImGui::Text("Graphics card: %s", glad_glGetString(GL_RENDERER));
		ImGui::Text("OpenGL version: %s", glad_glGetString(GL_VERSION));
		ImGui::Text("GLSL version: %s", glad_glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::End();
	}

	camera.display();

	if (view_imguidemo)
		ImGui::ShowDemoWindow(&view_imguidemo);

#ifdef BUILD_IMPLOT
	if (view_implotdemo)
		ImPlot::ShowDemoWindow(&view_implotdemo);
#endif

	if (view_messages) {
		ImGui::Begin("Messages", &view_messages);
		if (ImGui::Button("Info")) {
			mailbox::CreateInfo("Information");
		}
		ImGui::SameLine();
		if (ImGui::Button("Warn")) {
			mailbox::CreateWarn("Warning");
		}
		ImGui::SameLine();
		if (ImGui::Button("Error")) {
			mailbox::CreateError("There was an error!");
		}
		ImGui::SameLine();
		if (ImGui::Button("Progress")) {
			GRender::Progress* prog = mailbox::CreateProgress("Running...", cancelFunction);
			std::thread thr(testProgress, &prog->progress);
			thr.detach();
		}
		ImGui::SameLine();
		if (ImGui::Button("Timer")) {
			GRender::Timer* timer = mailbox::CreateTimer("Timing stuff...", cancelFunction);
			std::thread thr(testTimer, timer);
			thr.detach();
		}
		ImGui::End();
	}

	//////////////////////////////////////////////////////////////////////////////
	// Polymer control

	ImGui::Begin("Polymer");

	int32_t numBeads = (int32_t) poly.getNumBeads();
	if (ImGui::DragInt("numBeads", &numBeads, 1.0f, 1, 8192) && numBeads > 0) {
		float kuhn = poly.getKuhn();
		float radius = poly.getRadius();
		poly = polymer::Polymer(uint32_t(numBeads), kuhn);
		poly.getRadius() = radius;
	}

	float& radius = poly.getRadius();
	ImGui::DragFloat("radius", &radius, 0.1f, 0.1f, 5.0f, "%.2f");

	glm::vec3 color = poly.getSphereColor();
	if (ImGui::ColorEdit3("Spheres", glm::value_ptr(color)))
		poly.updateSphereColor(color);

	color = poly.getCylinderColor();
	if (ImGui::ColorEdit3("Connections", glm::value_ptr(color)))
		poly.updateCylinderColor(color);

	ImGui::ColorEdit3("Background", glm::value_ptr(bgColor));

	ImGui::End();
	

	//////////////////////////////////////////////////////////////////////////////
	/// Updating viewport
	ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_NoTitleBar);
	viewport_hover = ImGui::IsWindowHovered();

	// Check if it needs to resize
	ImVec2 port = ImGui::GetContentRegionAvail();
	ImGui::Image((void *)(uintptr_t)fbuffer.getID(), port, {0.0f, 1.0f}, {1.0f, 0.0f});

	glm::uvec2 view = fbuffer.getSize();
	glm::uvec2 uport{ uint32_t(port.x), uint32_t(port.y) };

	if (uport.x != view.x || uport.y != view.y) {
		fbuffer = GRender::Framebuffer(uport);
		camera.setAspectRatio(port.x / port.y);
	}

	ImGui::End();
}

void Sandbox::ImGuiMenuLayer(void) {
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Open...", "Ctrl+O")) {
			auto callback = [](const fs::path& path, void* ptr) -> void {
				GRender::mailbox::CreateInfo("Selected file: " + path.string()); 
			};
			GRender::dialog::OpenFile("Open file...", { "txt", "json" }, callback);
		}

		if (ImGui::MenuItem("Save...", "Ctrl+S")) {
			auto callback = [](const fs::path& path, void* ptr) -> void {
				GRender::mailbox::CreateInfo("Save file: " + path.string());
			};
			GRender::dialog::SaveFile("Save file...", {"txt", "json"}, callback);
		}

		if (ImGui::MenuItem("Open directory...", "Ctrl+D")) {
			auto callback = [](const fs::path& path, void* ptr) -> void {
				GRender::mailbox::CreateInfo("Open directory: " + path.string());
			};
			GRender::dialog::OpenDirectory("Open directory...", callback);
		}

		if (ImGui::MenuItem("Exit"))
			closeApp();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		// TODO: Do I really need this?
		if (GRender::DPI_FACTOR == 1) {
			if (ImGui::MenuItem("Set HIDPI"))
				scaleSizes(); // Toggle sizes
		} 
		else {
			if (ImGui::MenuItem("* Set HIDPI"))
				scaleSizes(); // Toggle sizes
		}

		if (ImGui::MenuItem("Camera controls"))
			camera.open();

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
