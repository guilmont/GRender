#include "renderer.h"


class Application : public Renderer
{
public:
	Application(void);
	~Application(void) = default;

	void onUserUpdate(float deltaTime) override;
	void ImGuiLayer(void) override;
	void ImGuiMenuLayer(void) override;

private:
	bool
		view_specs = false,
		view_imguidemo = false;

#ifdef BUILD_IMPLOT
	bool view_implotdemo = false;
#endif // BUILD_IMPLOT


};


int main(void)
{
	Application app;
	app.mainLoop();

	return EXIT_SUCCESS;
}

Application::Application(void)
{
	pout("Welcome to GChess!!");
	pout("Current path:", std::filesystem::current_path());

	initialize("GChess", 1200 * DPI_FACTOR, 800 * DPI_FACTOR);


}

void Application::onUserUpdate(float deltaTime)
{
	bool
		ctrl = (keyboard[Key::LEFT_CONTROL] == Event::PRESS) || (keyboard[Key::RIGHT_CONTROL] == Event::PRESS),
		A = keyboard['S'] == Event::PRESS,
		D = keyboard['D'] == Event::PRESS;

	if (ctrl & A)
		view_specs = true;

	if (ctrl & D)
		view_imguidemo = true;

#ifdef BUILD_IMPLOT
	
	bool P = keyboard['P'] == Event::PRESS;

	if (ctrl & P)
		view_implotdemo = true;

#endif 

}

void Application::ImGuiLayer(void)
{
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

void Application::ImGuiMenuLayer(void)
{
	if (ImGui::BeginMenu("File"))
	{
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
