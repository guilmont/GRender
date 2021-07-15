cmake_minimum_required(VERSION 3.10.0)

project(Template)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup runtime output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} template.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GRender)

target_compile_definitions(${PROJECT_NAME} PRIVATE _CR_SECURE_NO_WARNINGS)

file(GLOB SHADERS "assets/*")

file(COPY ${SHADERS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
