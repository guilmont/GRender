#include "grender.h"


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

};


GRender::Application* GRender::createApplication(void) { return new Sandbox(); }


Sandbox::Sandbox(void)
{
	pout("Welcome to my application!!");
	pout("Current path:", std::filesystem::current_path());

	initialize("Sandbox", 1200 * DPI_FACTOR, 800 * DPI_FACTOR);


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
		dialog.createDialog(GDialog::OPEN, "Open file...", { "txt", "json"}, nullptr,
			[](const std::string& path, void* ptr) -> void
			{
				pout("Selected path:", path);
			});
		

	if (ctrl & S)
		dialog.createDialog(GDialog::SAVE, "Save file...", { "txt", "json" }, nullptr,
			[](const std::string& path, void* ptr) -> void
			{
				pout("Selected path:", path);
			});


#ifdef BUILD_IMPLOT
	bool P = keyboard['P'] == GEvent::PRESS;

	if (ctrl & P)
		view_implotdemo = true;

#endif 

}

void Sandbox::ImGuiLayer(void)
{

	dialog.showDialog();

	if (view_specs)
	{
		ImGui::Begin("Specs", &view_specs);

		char buf[128] = { 0 };
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

}

void Sandbox::ImGuiMenuLayer(void)
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open..."))
			dialog.createDialog(GDialog::OPEN, "Open file...", { "txt", "json" }, nullptr,
					[](const std::string& path, void* ptr) -> void
					{
						pout("Selected path:", path);
					});


		if (ImGui::MenuItem("Save..."))
			dialog.createDialog(GDialog::SAVE, "Save file...", { "txt", "json" }, nullptr,
				[](const std::string& path, void* ptr) -> void
				{
					pout("Selected path:", path);
				});


		if (ImGui::MenuItem("Exit"))
			closeApp();

		ImGui::EndMenu();
	} // file-menu


	if (ImGui::BeginMenu("About"))
	{
		if (ImGui::MenuItem("Specs"))
			view_specs = true;

		if (ImGui::MenuItem("ImGui Demo"))
			view_imguidemo = true;

#ifdef BUILD_IMPLOT
		if (ImGui::MenuItem("ImPlot Demo"))
			view_implotdemo = true;
#endif

		ImGui::EndMenu();
	} // file-menu

}
