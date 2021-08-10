#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <functional>

#include <filesystem>
namespace fs = std::filesystem;

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

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

namespace GRender
{
	static void pout(void) { std::cout << std::endl; }

	template <typename TP, typename... Args>
	static void pout(TP var, Args &&...args)
	{
		std::cout << var << " ";
		pout(std::forward<Args>(args)...);
	}

	///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////

#ifndef __APPLE__
#ifdef _DEBUG
	static void glErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{

		//if (severity < GL_DEBUG_SEVERITY_LOW)
		//	return;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW:
			pout("GL_DEBUG_SEVERITY_LOW ::", type, " => ", message);
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			pout("GL_DEBUG_SEVERITY_MEDIUM ::", type, " => ", message);
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			pout("GL_DEBUG_SEVERITY_HIGH ::", type, " => ", message);
			break;
		case GL_DEBUG_TYPE_ERROR:
			pout("GL_DEBUG_TYPE_ERROR ::", type, " => ", message);
			break;
		}
	}
#endif
#endif

}