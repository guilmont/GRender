cmake_minimum_required(VERSION 3.16.0)
project(GRender)

option(GRENDER_IMPLOT "Build plotting utilities" OFF)
option(GRENDER_BUILD_EXAMPLE "Build example on how to use GRender" ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}") # Apparently there is a bug with MSVC and CMake and a default /W3. We change it by hand
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /w14263 /Za")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_CRT_SECURE_NO_WARNINGS")  # so it doesn't bother about std::getenv
endif()

# Setup vendors
add_subdirectory(vendor)

# Create library
add_library(GRender STATIC
	"src/application.cpp"
	"src/camera.cpp"
	"src/camera2D.cpp"
	"src/computeShader.cpp"
	"src/dialog.cpp"
	"src/events.cpp"
	"src/fonts.cpp"
	"src/framebuffer.cpp"
	"src/interactiveImage.cpp"
	"src/mailbox.cpp"
	"src/orbitalCamera.cpp"
	"src/quad.cpp"
	"src/shader.cpp"
	"src/storageBuffer.cpp"
	"src/texture.cpp"
	"src/utils.cpp"
	"src/viewport.cpp"

	"src/internal/dialogImpl.cpp"
	"src/internal/fontsImpl.cpp"
	"src/internal/OpenSans.cpp"

	"src/objects/cube.cpp"
	"src/objects/cylinder.cpp"
	"src/objects/object.cpp"
	"src/objects/sphere.cpp"
)

target_include_directories(GRender PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(GRender PRIVATE "${PROJECT_SOURCE_DIR}/include/GRender")

set(DEPENDENCIES glm glad glfw imgui stb_image)

if (GRENDER_IMPLOT)
	target_compile_definitions(GRender PUBLIC BUILD_IMPLOT)
	set(DEPENDENCIES ${DEPENDENCIES} implot)
endif()

target_link_libraries(GRender PUBLIC ${DEPENDENCIES})
target_precompile_headers(GRender PUBLIC "include/GRender/core.h")

if (GRENDER_BUILD_EXAMPLE)
	add_subdirectory("example")
endif()
