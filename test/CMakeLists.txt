cmake_minimum_required(VERSION 3.10.0)


project(Template)
add_executable(${PROJECT_NAME} template.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE GRender)
target_compile_definitions(${PROJECT_NAME} PRIVATE _CR_SECURE_NO_WARNINGS)

file(GLOB SHADERS "assets/*.glsl")

file(COPY ${SHADERS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)
