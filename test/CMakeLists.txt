cmake_minimum_required(VERSION 3.10.0)


add_executable(Application template.cpp)
target_link_libraries(Application PRIVATE GRenderer)