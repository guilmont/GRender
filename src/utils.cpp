#include "GRender/utils.h"

namespace GRender::utils {

void PerformanceDisplay(bool& view) {
	if (!view) { return; }

	ImGui::Begin("Performance", &view);
	ImGui::Text("FT: %.3f ms", 1000.0f * ImGui::GetIO().DeltaTime);
	ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
	ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
	ImGui::Text("Graphics card: %s", glGetString(GL_RENDERER));
	ImGui::Text("OpenGL version: %s", glGetString(GL_VERSION));
	ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::End();
}

/////////////////////////////////////////////////////////////////////////////////////////

void ViewWidgetsDemo(bool& view) {
	if (!view) { return; }
	ImGui::ShowDemoWindow(&view);
}

} // namespace utils

