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

static void gr_pout(void) { std::cout << std::endl; }

template <typename TP, typename ...Args>
static void gr_pout(TP var, Args &&...args)
{
	std::cout << var << " ";
	gr_pout(args...);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


#ifdef _DEBUG
static void glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{

	//if (severity < GL_DEBUG_SEVERITY_LOW)
	//	return;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:
		gr_pout("GL_DEBUG_SEVERITY_LOW ::", type, " => ", message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		gr_pout("GL_DEBUG_SEVERITY_MEDIUM ::", type, " => ", message);
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		gr_pout("GL_DEBUG_SEVERITY_HIGH ::", type, " => ", message);
		break;
	case GL_DEBUG_TYPE_ERROR:
		gr_pout("GL_DEBUG_TYPE_ERROR ::", type, " => ", message);
		break;

	}
}
#endif

