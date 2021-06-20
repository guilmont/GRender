cmake_minimum_required(VERSION 3.10.0)
project(GRender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(BUILD_IMPLOT "Build plotting utilities" ON)
option(DPI_FACTOR "Controls size of interface for high density monitors" OFF)
option(BUILD_TEST "Build template test for GRender" OFF)


# Setup runtime output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Setup vendors
add_subdirectory(vendor)

# Create library
add_library(GRender STATIC 
	"include/core.h"
	"include/events.h"       "include/eventsWrapper.h"
	"include/dialog.h"       "src/dialog.cpp"
	"include/shader.h"       "src/shader.cpp"
	"include/texture.h"      "src/texture.cpp"
	"include/camera.h"       "src/camera.cpp"
	"include/quad.h"         "src/quad.cpp"
	"include/framebuffer.h"  "src/framebuffer.cpp"
	"include/fonts.h"        "src/fonts.cpp"
	"include/application.h"     "src/application.cpp"
)

set(DEPENDENCIES glm glad glfw imgui)
target_include_directories(GRender PUBLIC "include")

file(COPY "assets/Open_Sans" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
install(DIRECTORY "assets/Open_Sans" DESTINATION assets)

#############################

if (BUILD_IMPLOT)
	target_compile_definitions(GRender PUBLIC BUILD_IMPLOT)
	set(DEPENDENCIES ${DEPENDENCIES} implot)
endif()

#############################

if (DPI_FACTOR)
	target_compile_definitions(GRender PUBLIC DPI_FACTOR=2)
else()
	target_compile_definitions(GRender PUBLIC DPI_FACTOR=1)
endif()

#############################

target_precompile_headers(GRender PUBLIC "include/core.h")
target_link_libraries(GRender PUBLIC ${DEPENDENCIES})


# Copying assets

###########################################################
## TEMPLATE TEST ##########################################

if (BUILD_TEST)
	add_subdirectory(test)
endif()
