cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ASSETS_DIR "assets/Template")

#######################################
project(Template)

add_executable(${PROJECT_NAME} 
	"template.cpp"   "polymer.cpp"   "polymer.h")

target_link_libraries(${PROJECT_NAME} PRIVATE GRender)
target_compile_definitions(${PROJECT_NAME} PRIVATE 
	_CRT_SECURE_NO_WARNINGS                    # To avoid some sprintf warnings
	INSTALL_PATH="${CMAKE_INSTALL_PREFIX}"
	ASSETS="${ASSETS_DIR}"
)

file(COPY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION ${CMAKE_SOURCE_DIR}/${ASSETS_DIR})
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/${ASSETS_DIR}/" 
	CONFIGURATIONS Release	
		DESTINATION ${CMAKE_INSTALL_PREFIX}/${ASSETS_DIR}
)

