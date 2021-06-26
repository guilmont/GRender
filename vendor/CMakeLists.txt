cmake_minimum_required(VERSION 3.10)


###########################################################
## STB_IMAGE ##############################################

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE "stb_image")

###########################################################
## GLM ####################################################

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "glm")

###########################################################
## GLAD ###################################################

add_library(glad STATIC "glad/src/glad.c")   
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})
target_include_directories(glad PUBLIC "glad/include")

###########################################################
## GLFW ###################################################

set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_VULKAN_STATIC OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw)

###########################################################
## IMGUI ##################################################

add_library(imgui 
    STATIC 
        "imgui/imgui.cpp"
        "imgui/imgui_draw.cpp"
        "imgui/imgui_widgets.cpp"
        "imgui/imgui_tables.cpp"
        "imgui/imgui_demo.cpp"
        "imgui/backends/imgui_impl_glfw.cpp"
        "imgui/backends/imgui_impl_opengl3.cpp"
)

target_link_libraries(imgui PUBLIC glad glfw)
target_include_directories(imgui PUBLIC "imgui" "imgui/backends")
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)


###########################################################
## IMPLOT #################################################

if (BUILD_IMPLOT)
    add_library(implot 
        STATIC 
            "implot/implot.cpp"
            "implot/implot_demo.cpp"
            "implot/implot_items.cpp"
    )

    target_include_directories(implot PUBLIC "implot")
    target_link_libraries(implot PUBLIC imgui)

endif()
