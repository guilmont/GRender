#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>

// vendor
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#ifdef BUILD_IMPLOT
	#include <implot.h>
#endif


namespace fs = std::filesystem;

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

#ifdef WIN32
	#define GAPI __declspec(dllexport)
#else
	#define GAPI
#endif


/////////////////////////////

static void pout(void) { std::cout << std::endl; }

template <typename TP, typename ...Args>
static void pout(TP var, Args &&...args)
{
	std::cout << var << " ";
	pout(args...);
}