#pragma once

#include "application.h"

extern GRender::Application* GRender::createApplication(int argc, char** argv);

#if defined(WIN32) && !defined(_DEBUG)
#define NOMINMAX  // Otherwise Windows.h win include definition to min/max, conflicting with std::min/max
#define WIN32_LEAN_AND_MEAN  // Removing other definitions that we don't need and would increase size of executable
#include <Windows.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
	GRender::Application* app = GRender::createApplication(__argc, __argv);
#else
int main(int argc, char *argv[]) {
	GRender::Application* app = GRender::createApplication(argc, argv);
#endif

	app->run();
	delete app;

	return EXIT_SUCCESS;
}
