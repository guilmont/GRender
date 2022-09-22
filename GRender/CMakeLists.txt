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
	"src/events.cpp"
	"src/dialog.cpp"
	"src/shader.cpp"
	"src/texture.cpp"
	"src/camera.cpp"
	"src/camera2D.cpp"
	"src/quad.cpp"
	"src/framebuffer.cpp"
	"src/fonts.cpp"           
	"src/application.cpp"
	"src/mailbox.cpp"
	"src/viewport.cpp"

	"src/internal/dialogImpl.cpp"
	"src/internal/fontsImpl.cpp"
	"src/internal/OpenSans.cpp"

	"src/objects/object.cpp"
	"src/objects/cube.cpp"
	"src/objects/sphere.cpp"
)

target_include_directories(GRender PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(GRender PRIVATE "${PROJECT_SOURCE_DIR}/include/GRender")
target_compile_definitions(GRender PRIVATE _CRT_SECURE_NO_WARNINGS)  # so it doesn't bother about std::getenv

set(DEPENDENCIES glm glad glfw imgui stb_image)

if (GRENDER_IMPLOT)
	target_compile_definitions(GRender PUBLIC BUILD_IMPLOT)
	set(DEPENDENCIES ${DEPENDENCIES} implot)
endif()

target_link_libraries(GRender PUBLIC ${DEPENDENCIES})
target_precompile_headers(GRender PUBLIC "include/GRender/core.h")
