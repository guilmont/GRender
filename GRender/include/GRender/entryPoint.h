#pragma once

#include "application.h"

extern GRender::Application* GRender::createApplication(int argc, char** argv);

#if defined(WIN32) && !defined(_DEBUG)
#define NOMINMAX  // Otherwise Windows.h win include definition to min/max, conflicting with std::min/max
#include <Windows.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int) {
	GRender::Application* app = GRender::createApplication(__argc, __argv);
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#else
int main(int argc, char *argv[]) {
	GRender::Application* app = GRender::createApplication(argc, argv);
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#endif