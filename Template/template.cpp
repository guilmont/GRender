#include "GRender.h"
#include "polymer.h"

#include <random>

namespace fs = std::filesystem;

class Sandbox : public GRender::Application
{
public:
	Sandbox(void);
	~Sandbox(void) = default;

	void onUserUpdate(float deltaTime) override;
	void ImGuiLayer(void) override;
	void ImGuiMenuLayer(void) override;

private:
	bool
		view_specs = false,
		view_imguidemo = false,
		viewport_hover = false;

#ifdef BUILD_IMPLOT
	bool view_implotdemo = false;
#endif


	GRender::Camera camera;
	GRender::Shader shader;
	GRender::Framebuffer fbuffer;

	std::unique_ptr<Polymer> poly = nullptr;

	void generatePolymer(uint32_t numBeads, float kuhn);

	glm::vec3 bgColor = { 0.3f, 0.3f, 0.3f };

};

GRender::Application* GRender::createApplication() {
	return new Sandbox;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Sandbox::Sandbox(void) : Application("Sandbox", 1200, 800, "layout.ini") {
	std::filesystem::path assets(ASSETS);
	shader.loadShader("polyBlobs",
					  assets / "polyBlobs.vtx.glsl",
					  assets / "polyShader.frag.glsl");


	shader.loadShader("polyConnections",
					  assets / "polyConnections.vtx.glsl",
					  assets / "polyShader.frag.glsl");
	
	fbuffer = GRender::Framebuffer(1200, 800);

	glEnable(GL_DEPTH_TEST);
	generatePolymer(1024, 1.0f);

}

void Sandbox::generatePolymer(uint32_t numBeads, float kuhn)
{
	uint32_t resolution = 50;

	std::default_random_engine ran(123456);
	std::normal_distribution<float> normal(0.0f, kuhn / sqrt(3.0f));

	glm::vec3 com = { 0.0f, 0.0f, 0.0f };
	std::vector<glm::vec3> vpos(numBeads);

	vpos.at(0) = { 0.0f, 0.0f, 0.0f };
	for (uint64_t k = 1; k < numBeads; k++)
	{
		vpos.at(k).x = vpos.at(k - 1).x + normal(ran);
		vpos.at(k).y = vpos.at(k - 1).y + normal(ran);
		vpos.at(k).z = vpos.at(k - 1).z + normal(ran);
		com += vpos.at(k);
	}

	com /= float(numBeads);
	for (glm::vec3& p : vpos)
		p -= com;

	poly = std::make_unique<Polymer>(numBeads, resolution);
	poly->kuhn = kuhn;
	
	poly->update(vpos);
}

void Sandbox::onUserUpdate(float deltaTime)
{
	bool
		ctrl = keyboard.isDown(GRender::Key::LEFT_CONTROL) || keyboard.isDown(GRender::Key::RIGHT_CONTROL),
		shift = keyboard.isDown(GRender::Key::LEFT_SHIFT) || keyboard.isDown(GRender::Key::RIGHT_SHIFT);

	if (ctrl && keyboard.isPressed('H'))
		view_specs = true;

	if (ctrl && keyboard.isPressed('D'))
		view_imguidemo = true;

	if (ctrl && keyboard.isPressed('O'))
		dialog.createDialog(GDialog::OPEN, "Open file...", {"txt", "json"}, &mailbox,
							[](const fs::path &path, void *ptr) -> void{
								std::cout << "Selected path: " << path << std::endl;
								reinterpret_cast<GRender::Mailbox*>(ptr)->createInfo("Selected file: " + path.string());
							});

	if (ctrl && keyboard.isPressed('S'))
		dialog.createDialog(GDialog::SAVE, "Save file...", {"txt", "json"}, nullptr,
							[](const fs::path &path, void *ptr) -> void
							{ std::cout << "Selected path: " << path << std::endl; });

#ifdef BUILD_IMPLOT
	if (ctrl && keyboard.isPressed('P'))
		view_implotdemo = true;
#endif

	///////////////////////////////////////////////////////////////////////////
	// Camera
	if (viewport_hover)
	{
		if ((keyboard.isDown('W')) || (mouse.wheel() > 0.0f))
			camera.moveFront(deltaTime);
		
		if ((keyboard.isDown('S')) || (mouse.wheel() < 0.0f))
			camera.moveBack(deltaTime);

		if (keyboard.isDown('D'))
			camera.moveRight(deltaTime);

		if (keyboard.isDown('A'))
			camera.moveLeft(deltaTime);

		if (keyboard.isDown('E'))
			camera.moveUp(deltaTime);

		if (keyboard.isDown('Q'))
			camera.moveDown(deltaTime);


		if (mouse.isClicked(GRender::MouseButton::MIDDLE))
			camera.reset();

		if (mouse.isPressed(GRender::MouseButton::LEFT))
			camera.lookAround(mouse.delta(), deltaTime);
	}

	///////////////////////////////////////////////////////////////////////////

	fbuffer.bind();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);

	// Drawing polymer
	shader.useProgram("polyConnections");
	shader.setMatrix4f("u_transform", glm::value_ptr(camera.getViewMatrix()));
	shader.setFloat("u_radius", 0.25f*poly->kuhn);
	poly->submitConnections();
	
	shader.useProgram("polyBlobs");
	shader.setMatrix4f("u_transform", glm::value_ptr(camera.getViewMatrix()));
	shader.setFloat("u_radius", 0.5f*poly->kuhn);
	poly->submitBlobs();

	fbuffer.unbind();
}

void Sandbox::ImGuiLayer(void)
{

	if (view_specs)
	{
		ImGui::Begin("Specs", &view_specs);
		ImGui::Text("FT: %.3f ms", 1000.0 * double(ImGui::GetIO().DeltaTime));
		ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::Text("Vendor: %s", glad_glGetString(GL_VENDOR));
		ImGui::Text("Graphics card: %s", glad_glGetString(GL_RENDERER));
		ImGui::Text("OpenGL version: %s", glad_glGetString(GL_VERSION));
		ImGui::Text("GLSL version: %s", glad_glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::End();
	}

	if (camera.viewControls)
		camera.controls();

	if (view_imguidemo)
		ImGui::ShowDemoWindow(&view_imguidemo);

#ifdef BUILD_IMPLOT
	if (view_implotdemo)
		ImPlot::ShowDemoWindow(&view_implotdemo);
#endif

	//////////////////////////////////////////////////////////////////////////////
	// Polymer control

	ImGui::Begin("Polymer");

	int32_t numBeads = int32_t(poly->numBeads);
	float kuhn = poly->kuhn;

	if (ImGui::DragInt("numBeads", &numBeads, 1.0f, 1, 8192))
		generatePolymer(numBeads, kuhn);

	ImGui::DragFloat("radius", &(poly->kuhn), 0.1f, 0.1f, 5.0f, "%.2f");

	if (ImGui::ColorEdit3("Spheres", glm::value_ptr(poly->sphereColor)))
		poly->updateSphereColor();

	if (ImGui::ColorEdit3("Connections", glm::value_ptr(poly->cylinderColor)))
		poly->updateCylinderColor();

	if (ImGui::ColorEdit3("Background", glm::value_ptr(bgColor)))
		poly->updateCylinderColor();

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

void Sandbox::ImGuiMenuLayer(void)
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open...", "Ctrl+O"))
			dialog.createDialog(GDialog::OPEN, "Open file...", {"txt", "json"}, &mailbox,
								[](const fs::path &path, void *ptr) -> void {
									std::cout << "Selected path: " << path << std::endl;
									reinterpret_cast<GRender::Mailbox*>(ptr)->createInfo("Selected file: " + path.string());
								});

		if (ImGui::MenuItem("Save...", "Ctrl+S"))
			dialog.createDialog(GDialog::SAVE, "Save file...", {"txt", "json"}, nullptr,
								[](const fs::path &path, void *ptr) -> void { std::cout << "Selected path: " << path << std::endl; });

		if (ImGui::MenuItem("Exit"))
			closeApp();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		if (GRender::DPI_FACTOR == 1) {
			if (ImGui::MenuItem("Set HIDPI")) {
				scaleSizes(2.0f);
			}
		} 
		else {
			if (ImGui::MenuItem("* Set HIDPI")) {
				scaleSizes(1.0f);
			}
		}

		if (ImGui::MenuItem("Camera controls"))
			camera.viewControls = true;

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

		if (ImGui::MenuItem("View mailbox"))
			mailbox.setActive();

		
		ImGui::EndMenu();
	}
}
