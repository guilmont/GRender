cmake_minimum_required(VERSION 3.10.0)


project(Test)
add_executable(Application template.cpp)
target_link_libraries(Application PRIVATE GRender)
target_compile_definitions(Application PRIVATE _CR_SECURE_NO_WARNINGS)

file(GLOB SHADERS "assets/*.glsl")

file(COPY ${SHADERS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

if (WIN32)
	set_property(TARGET Application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()	