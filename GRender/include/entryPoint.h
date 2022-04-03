#pragma once

extern GRender::Application* GRender::createApplication();

#ifdef WIN32
#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int) {
	GRender::Application* app = GRender::createApplication();
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#else
int main(int argc, char *argv[]) {
	// Setup program to use executable path as reference
	fs::path exe(argv[0]);
	fs::current_path(exe.parent_path());

	GRender::Application* app = GRender::createApplication();
	app->run();
	delete app;

	return EXIT_SUCCESS;
}

#endif