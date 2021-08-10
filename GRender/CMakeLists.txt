cmake_minimum_required(VERSION 3.10.0)
project(GRender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GRENDER_IMPLOT "Build plotting utilities" ON)
option(GRENDER_PRECOMPILE_HEADERS "Create precompiled headers" OFF)
option(GRENDER_DPI_FACTOR "Controls size of interface for high density monitors" OFF)

# Setup vendors
add_subdirectory(vendor)

# Create library
add_library(${PROJECT_NAME} STATIC 
	"include/core.h"
	"include/events.h"       "include/eventsWrapper.h"
	"include/dialog.h"       "src/dialog.cpp"
	"include/shader.h"       "src/shader.cpp"
	"include/texture.h"      "src/texture.cpp"
	"include/camera.h"       "src/camera.cpp"
	"include/quad.h"         "src/quad.cpp"
	"include/framebuffer.h"  "src/framebuffer.cpp"
	"include/fonts.h"        "src/fonts.cpp"
	"include/application.h"  "src/application.cpp"
)

set(DEPENDENCIES glm glad glfw imgui stb_image)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(${PROJECT_NAME} PRIVATE 
	_CRT_SECURE_NO_WARNINGS    # To avoid some sprintf warnings
	ASSETS="${ASSETS_DIR}"
)

file(COPY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION "${CMAKE_SOURCE_DIR}/${ASSETS_DIR}")

#############################

if (GRENDER_IMPLOT)
	target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_IMPLOT)
	set(DEPENDENCIES ${DEPENDENCIES} implot)
endif()

#############################

if (GRENDER_DPI_FACTOR)
	target_compile_definitions(${PROJECT_NAME} PUBLIC DPI_FACTOR=2)
else()
	target_compile_definitions(${PROJECT_NAME} PUBLIC DPI_FACTOR=1)
endif()

#############################

if (GRENDER_PRECOMPILE_HEADERS)
	target_precompile_headers(${PROJECT_NAME} PUBLIC "include/core.h")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${DEPENDENCIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS="${ASSETS_DIR}")
