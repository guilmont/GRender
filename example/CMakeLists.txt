cmake_minimum_required(VERSION 3.10)
project(Template)

# We will generate all the executable in the same location for simplicity
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(Template	
	"template.cpp"
	"polymer.cpp"
)
target_link_libraries(Template PRIVATE GRender)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(Template PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
	set_target_properties(Template PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
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




