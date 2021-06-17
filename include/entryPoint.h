#pragma once

extern Renderer* createApplication();


int main(int , char**)
{
	auto app = createApplication();
	app->mainLoop();
	delete app;
}