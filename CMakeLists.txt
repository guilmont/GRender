cmake_minimum_required(VERSION 3.20)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(ASSETS_DIR "assets/Template")

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Location to install application")


project(Application)

# Including Grender library
add_subdirectory(GRender)

option(GRENDER_BUILD_TEMPLATE "Builds template" OFF)

if (GRENDER_BUILD_TEMPLATE)
    add_subdirectory(Template)
endif()