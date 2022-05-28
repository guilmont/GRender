#pragma once

extern GRender::Application* GRender::createApplication(int argc, char** argv);

#if defined(WIN32) && !defined(_DEBUG)
#include <Windows.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
	namespace fs = std::filesystem;

	// Setup program to use executable path as reference
	std::filesystem::path exe = fs::path{ __argv[0] }.parent_path();
	if (fs::exists(exe))
		fs::current_path(exe);

	GRender::Application* app = GRender::createApplication(__argc, __argv);
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#else
int main(int argc, char *argv[]) {
	// Setup program to use executable path as reference
	std::filesystem::path exe(argv[0]);
	std::filesystem::current_path(exe.parent_path());

	GRender::Application* app = GRender::createApplication(argc, argv);
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#endif