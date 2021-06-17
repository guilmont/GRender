cmake_minimum_required(VERSION 3.10.0)
project(GRenderer VERSION 0.1.0)

# Setup vendors
add_subdirectory(vendor)

# Creating chess library
file(GLOB SRC_FILES "src/*.cpp")
file(GLOB INC_FILES "include/*.h")
file(GLOB ASSETS "assets/*")

add_library(Renderer SHARED ${SRC_FILES} ${INC_FILES})
target_include_directories(Renderer PUBLIC "include")
target_link_libraries(Renderer PUBLIC glm glad glfw imgui)

target_compile_definitions(Renderer PUBLIC DPI_FACTOR=1)

file(COPY ${ASSETS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)