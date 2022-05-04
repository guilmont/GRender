cmake_minimum_required(VERSION 3.10.0)
project(GRender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")


option(GRENDER_IMPLOT "Build plotting utilities" ON)

# Setup vendors
add_subdirectory(vendor)

# Create library
add_library(GRender STATIC 
	"include/core.h"
	"include/eventsWrapper.h"      "include/events.h"         "src/events.cpp"
	"src/internal/dialogImpl.h"    "src/dialog.cpp"
	"include/shader.h"             "src/shader.cpp"
	"include/texture.h"            "src/texture.cpp"
	"include/camera.h"             "src/camera.cpp"
	"include/camera2D.h"           "src/camera2D.cpp"
	"include/quad.h"               "src/quad.cpp"
	"include/framebuffer.h"        "src/framebuffer.cpp"
	"include/fonts.h"              "src/fonts.cpp"           
	"include/application.h"        "src/application.cpp"
	"include/mailbox.h"            "src/mailbox.cpp"
	"include/entryPoint.h"
    "include/table.h"

	"src/internal/dialogImpl.h"    "src/internal/dialogImpl.cpp"
	"src/internal/fontsImpl.cpp"   "src/internal/fontsImpl.cpp"      "src/internal/OpenSans.cpp"
)

target_include_directories(GRender PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_definitions(GRender PRIVATE _CRT_SECURE_NO_WARNINGS)  # so it doesn't bother about std::getenv

set(DEPENDENCIES glm glad glfw imgui stb_image)

if (GRENDER_IMPLOT)
	target_compile_definitions(GRender PUBLIC BUILD_IMPLOT)
	set(DEPENDENCIES ${DEPENDENCIES} implot)
endif()


target_link_libraries(GRender PUBLIC ${DEPENDENCIES})
target_precompile_headers(GRender PUBLIC "include/core.h")

