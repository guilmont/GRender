cmake_minimum_required(VERSION 3.10)

# This CMakeLists.txt will only setup the template in relation to GRender
# For other purpoes, add directly the subdirectory GRender

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# A trick so the program can always find its assets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
set(ASSETS_DIR "assets")  # GRender need to know this path. Written in relation source dir

add_subdirectory("GRender")

##############################################################

project(Template)

add_executable(${PROJECT_NAME} "Template/template.cpp"   "Template/polymer.cpp"   "Template/polymer.h")

target_link_libraries(${PROJECT_NAME} PRIVATE GRender)
target_compile_definitions(${PROJECT_NAME} PRIVATE ASSETS="../${ASSETS_DIR}")

if (WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

file(GLOB PROJ_ASSETS "${CMAKE_SOURCE_DIR}/Template/assets/*")
file(COPY ${PROJ_ASSETS} DESTINATION "${CMAKE_SOURCE_DIR}/${ASSETS_DIR}")

### INTSTALLATION ##########################################

install(TARGETS "${PROJECT_NAME}" 
	CONFIGURATIONS Release
		DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)

install(DIRECTORY "${ASSETS_DIR}" 
	CONFIGURATIONS Release	
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${ASSETS_DIR}
)




