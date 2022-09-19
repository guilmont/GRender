#include <random>
#include <thread>

#include "GRender/application.h"
#include "GRender/entryPoint.h"

#include "GRender/camera.h"
#include "GRender/quad.h"
#include "GRender/shader.h"
#include "GRender/table.h"
#include "GRender/viewport.h"

#include "polymer.h"
#include "GRender/objects/cube.h"


namespace fs = std::filesystem;

using TexSpecification = GRender::texture::Specification;

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
	GRender::Viewport view;
	GRender::Table<GRender::Shader> shader;
	GRender::Texture texture;

	GRender::Quad quad;
	polymer::Polymer poly;

	GRender::Cube cube;

	glm::vec3 bgColor = { 0.3f, 0.3f, 0.3f };

};

GRender::Application* GRender::createApplication(int argc, char** argv) {
	// Setup program to use executable path as reference
	namespace fs = std::filesystem;

	fs::path exe = fs::path{ argv[0] }.parent_path();
	if (fs::exists(exe))
		fs::current_path(exe);

	WARN("Current path: " + fs::current_path().string());

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
	shader.insert("quad",            { assets / "quad.vtx.glsl",            assets / "quad.frag.glsl"       });
	shader.insert("objects",         { assets / "objects.vtx.glsl",         assets / "objects.frag.glsl"    });

	TexSpecification defSpec;
	view = GRender::Viewport({ 1200, 800 }, { defSpec, defSpec }, true);
	camera = GRender::Camera({ 0.0f, 5.0f, 20.0f }, -0.18f, -1.5708f);
	camera.open();

	quad = GRender::Quad(1);
	cube = GRender::Cube(2);

	using namespace GRender::texture;
	Specification spec;
	spec.wrap.x = Wrap::MIRRORED;
	spec.wrap.y = Wrap::REPEAT;
	texture = GRender::utils::createTextureFromRGBAFile("../assets/space.jpg", spec);

	glEnable(GL_DEPTH_TEST);
	poly = polymer::Polymer(128, 1.0f);
}

void Sandbox::onUserUpdate(float deltaTime) {
	using namespace GRender;

	bool ctrl = keyboard::IsDown(Key::LEFT_CONTROL) || keyboard::IsDown(Key::RIGHT_CONTROL);

	if (ctrl && keyboard::IsPressed('H'))
		view_specs = true;

	if (ctrl && keyboard::IsPressed('I'))
		view_imguidemo = true;

	if (ctrl && keyboard::IsPressed('O')) {
		auto callback = [](const fs::path& path, void*) -> void { mailbox::CreateInfo("Selected file: " + path.string()); };
		dialog::OpenFile("Open file...", { "txt", "json" }, callback);
	}

	if (ctrl && keyboard::IsPressed('S')) {
		auto callback = [](const fs::path& path, void*) -> void { mailbox::CreateInfo("Save file: " + path.string()); };
		dialog::SaveFile("Save file...", {"txt", "json"}, callback);
	}

	if (ctrl && keyboard::IsPressed('D')) {
		auto callback = [](const fs::path& path, void*) -> void {
			mailbox::CreateInfo("Open directory: " + path.string());
		};
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
	if (view.hovered() && !ctrl)  // !ctrl not to interfere with commands used above
		camera.controls(deltaTime);
	
	///////////////////////////////////////////////////////////////////////////

	view.bind();
	
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

#if true
	auto& qsh = shader["quad"].bind();
	texture.bind();
	qsh.setInteger("texSampler", 0);
	qsh.setMatrix4f("u_transform", glm::value_ptr(camera.getViewMatrix()));

	static float tt = 0;
	tt += deltaTime;
	auto sz = texture.size();

	quad::Specification spec;
	spec.position = { 5.0f, cos(tt), 0.0f};
	spec.size = glm::vec2{ float(sz.x) / float(sz.y), 1.0f };
	spec.texCoord = glm::vec4{ 0.0f, 0.0f, 2.0f, 2.0f };
	spec.texID = texture.id();
	quad.submit(spec);
	quad.draw();

	// Drawing cube
	Shader& osh = shader["objects"].bind();
	osh.setMatrix4f("u_transform", glm::value_ptr(camera.getViewMatrix()));

	object::Specification obj;
	obj.position = { 0.0f, cos(tt), 0.0f };
	obj.rotation.z = 0.5f * tt;
	obj.color = { 1.0f, 0.5f, 0.3f, 1.0f };
	cube.submit(obj);

	obj.rotation = { 0.0f, tt, 0.0f };
	obj.position = {cos(tt), 2.0f, 0.0f };
	obj.scale.x = 1.0 + 0.7f * cos(tt);
	cube.submit(obj);
	cube.draw();

#endif 

	view.unbind();
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

	static bool Attach = false;
	if (ImGui::Button("Switch attachment"))
		Attach = !Attach;

	ImGui::End();
	

	// Show viewport
	view.display("Viewport", Attach);

	// Update camera aspect ratio in case viewport changed
	glm::vec2 vsz= view.size();
	camera.setAspectRatio(vsz.x / vsz.y);
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
