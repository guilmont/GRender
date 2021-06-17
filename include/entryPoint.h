#pragma once


extern GRender::Application* GRender::createApplication(void);


int main(void)
{
	auto app = GRender::createApplication();
	app->mainLoop();
	delete app;

	return 0;
}