#pragma once

// This is a precompiled header

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
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

namespace GRender {

namespace fs = std::filesystem;

inline float DPI_FACTOR = 1;  // Used to rescale sizes for HIDPI screens

#if defined(_DEBUG)
	#define ASSERT(EXPR, MSG) if (!(EXPR)) { std::cerr << "ERROR (" << __FUNCTION__ << "): "<< (MSG) << std::endl; std::abort(); }
	#define WARN(MSG)         std::cout << "WARN (" << __FUNCTION__ << "): "<< (MSG) << std::endl;
#else 
	#define ASSERT(EXPR, MSG) {}
	#define WARN(MSG)         {}
#endif

#if defined(_DEBUG) && !defined(__APPLE__) // Apple is stuck with openGL 4.1
static void glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
							GLsizei length, const GLchar *message, const void *userParam) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cerr << "GL_DEBUG_SEVERITY_LOW :: " << type << " => " << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "GL_DEBUG_SEVERITY_MEDIUM :: " << type << " => " << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "GL_DEBUG_SEVERITY_HIGH :: " << type << " => " << message << std::endl;
		break;
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "GL_DEBUG_TYPE_ERROR :: " << type << " => " << message << std::endl;
		break;
	}
}
#endif

}