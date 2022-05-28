cmake_minimum_required(VERSION 3.10)

# This CMakeLists.txt will only setup the template in relation to GRender
# For other purpoes, add directly the subdirectory GRender

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# A trick so the program can always find its assets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory("GRender")

##############################################################

project(Template)

add_executable(Template "Template/template.cpp"   "Template/polymer.cpp")

target_link_libraries(Template PRIVATE GRender)

if (WIN32)
	set_target_properties(Template PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

### INTSTALLATION ##########################################

install(TARGETS "Template" 
	CONFIGURATIONS Release
		DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)

install(DIRECTORY "assets" 
	CONFIGURATIONS Release	
		DESTINATION ${CMAKE_INSTALL_PREFIX}
)




