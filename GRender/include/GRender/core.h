#pragma once

// This is a precompiled header

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
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

#if !defined(NDEBUG)
	#define ASSERT(EXPR, MSG) if (!(EXPR)) { std::cerr << "\033[1;31mERROR (\033[0m\033[1m" << __FUNCTION__ << "\033[1;31m):\033[0m "<< (MSG) << std::endl; std::abort(); }
	#define WARN(MSG) std::cout << "\033[1;33mWARN (\033[0m\033[1m" << __FUNCTION__ << "\033[1;33m):\033[0m "<< (MSG) << std::endl
	#define INFO(MSG) std::cout << "\033[1;32mINFO (\033[0m\033[1m" << __FUNCTION__ << "\033[1;32m):\033[0m "<< (MSG) << std::endl
#else 
	#define ASSERT(EXPR, MSG) {}
	#define WARN(MSG)         {}
	#define INFO(MSG)         {}
#endif

#if !defined(NDEBUG) && !defined(__APPLE__) // Apple is stuck with openGL 4.1
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