#include "GRender.h"

constexpr uint32_t maxQuads = 500;

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
		view_imguidemo = false;

#ifdef BUILD_IMPLOT
	bool view_implotdemo = false;
#endif

	std::unique_ptr<GRender::Framebuffer> fbuffer = nullptr;
	std::unique_ptr<GRender::Quad> quad = nullptr;

	float angle = 0.0f;
	std::array<glm::vec3, maxQuads> vecPos;
};

int main(int argc, char *argv[])
{

	if (argc > 1)
		GRender::pout(argv[1]);


	Sandbox *app = new Sandbox();
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

Sandbox::Sandbox(void)
{
	fs::current_path(INSTALL_PATH); // We set the execution path so we don't need to worry about relative paths

	uint32_t width = 1200, height = 800;
	initialize("Sandbox", width, height);

	fs::path assets(ASSETS);

	// Setup shader path
	shader.loadShader("quadShader",
					  assets / "quadShader.vtx.glsl",
					  assets / "quadShader.frag.glsl");

	fbuffer = std::make_unique<GRender::Framebuffer>(width, height);
	quad = std::make_unique<GRender::Quad>(maxQuads);

	uint32_t white[] = {0xffffffff};
	texture.createRGBA("default", 1, 1, white);
	texture.createRGBA("wall", assets / "jessica.jpg");

	for (auto &quad : vecPos)
		quad = {0.0f, 0.0f, 0.0f};
}

void Sandbox::onUserUpdate(float deltaTime)
{
	bool
		ctrl = (keyboard[GKey::LEFT_CONTROL] == GEvent::PRESS) || (keyboard[GKey::RIGHT_CONTROL] == GEvent::PRESS),
		H = keyboard['H'] == GEvent::PRESS,
		D = keyboard['D'] == GEvent::PRESS,
		O = keyboard['O'] == GEvent::PRESS,
		S = keyboard['S'] == GEvent::PRESS;

	if (ctrl & H)
		view_specs = true;

	if (ctrl & D)
		view_imguidemo = true;

	if (ctrl & O)
		dialog.createDialog(GDialog::OPEN, "Open file...", {"txt", "json"}, nullptr,
							[](const std::string &path, void *ptr) -> void
							{ GRender::pout("Selected path:", path); });

	if (ctrl & S)
		dialog.createDialog(GDialog::SAVE, "Save file...", {"txt", "json"}, nullptr,
							[](const std::string &path, void *ptr) -> void
							{ GRender::pout("Selected path:", path); });

#ifdef BUILD_IMPLOT
	bool P = keyboard['P'] == GEvent::PRESS;

	if (ctrl & P)
		view_implotdemo = true;

#endif

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

	auto random = [](float a = 0.0f, float b = 1.0f) -> float
	{ return a + (b - a) * float(rand()) / float(RAND_MAX); };

	for (auto &pos : vecPos)
	{
		pos.x += random(-0.01f, 0.01f);
		pos.y += random(-0.01f, 0.01f);

		glm::vec2 size = {random(0.05f, 0.1f), random(0.05f, 0.1f)};
		glm::vec4 cor = {random(), random(), random(), 1.0f};

		quad->draw(pos, size, 0.0f, 1.0f);
	}

	fbuffer->bind();

	glad_glClear(GL_COLOR_BUFFER_BIT);
	glad_glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	shader.useProgram("quadShader");
	shader.setMatrix4f("u_viewProjection", glm::value_ptr(camera.getViewMatrix()));

	texture.bind("default", 0);
	texture.bind("wall", 1);

	int32_t tex[2] = {0, 1};

	shader.setIntArray("u_texture", tex, 2);

	quad->submit();

	fbuffer->unbind();
}

void Sandbox::ImGuiLayer(void)
{

	if (view_specs)
	{
		ImGui::SetNextWindowSize({512 * DPI_FACTOR, 300 * DPI_FACTOR});

		ImGui::Begin("Specs", &view_specs);

		char buf[128] = {0};
		sprintf(buf, "FT: %.3f ms", 1000.0 * double(ImGui::GetIO().DeltaTime));
		ImGui::Text(buf);

		sprintf(buf, "FPS: %.0f", ImGui::GetIO().Framerate);
		ImGui::Text(buf);

		sprintf(buf, "Vendor: %s", glad_glGetString(GL_VENDOR));
		ImGui::Text(buf);

		sprintf(buf, "Graphics card: %s", glad_glGetString(GL_RENDERER));
		ImGui::Text(buf);

		sprintf(buf, "OpenGL version: %s", glad_glGetString(GL_VERSION));
		ImGui::Text(buf);

		sprintf(buf, "GLSL version: %s", glad_glGetString(GL_SHADING_LANGUAGE_VERSION));
		ImGui::Text(buf);

		ImGui::End();
	}

	if (view_imguidemo)
		ImGui::ShowDemoWindow(&view_imguidemo);

#ifdef BUILD_IMPLOT
	if (view_implotdemo)
		ImPlot::ShowDemoWindow(&view_implotdemo);
#endif

	//////////////////////////////////////////////////////////////////////////////
	/// Updating viewport

	glm::vec2 size = 0.75f * window.size;
	ImGui::SetNextWindowSize({size.x, size.y});

	ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_NoTitleBar);

	// Check if it needs to resize
	ImVec2 port = ImGui::GetContentRegionAvail();
	ImGui::Image((void *)(uintptr_t)fbuffer->getID(), port, {0.0f, 1.0f}, {1.0f, 0.0f});

	glm::vec2 view = fbuffer->getSize();
	if (port.x != view.x || port.y != view.y)
	{
		fbuffer = std::make_unique<GRender::Framebuffer>(uint32_t(port.x), uint32_t(port.y));
		camera.setAspectRatio(port.x / port.y);
	}

	// Checking if anchoring position changed
	ImVec2 pos = ImGui::GetItemRectMin();
	fbuffer->setPosition(pos.x - window.position.x, pos.y - window.position.y);

	ImGui::End();
}

void Sandbox::ImGuiMenuLayer(void)
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open...", "Ctrl+O"))
			dialog.createDialog(GDialog::OPEN, "Open file...", {"txt", "json"}, nullptr,
								[](const std::string &path, void *ptr) -> void
								{
									GRender::pout("Selected path:", path);
								});

		if (ImGui::MenuItem("Save...", "Ctrl+S"))
			dialog.createDialog(GDialog::SAVE, "Save file...", {"txt", "json"}, nullptr,
								[](const std::string &path, void *ptr) -> void
								{
									GRender::pout("Selected path:", path);
								});

		if (ImGui::MenuItem("Exit"))
			closeApp();

		ImGui::EndMenu();
	} // file-menu

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

		ImGui::EndMenu();
	} // file-menu
}
