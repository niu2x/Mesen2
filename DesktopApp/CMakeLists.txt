cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(DesktopApp VERSION 0.1.0 LANGUAGES CXX C)

find_package(glfw3 REQUIRED)

add_executable(Childhood-Fun "")

file(GLOB_RECURSE imgui_src src/imgui/*.cpp)

target_sources(Childhood-Fun PRIVATE
    src/main.cpp
    src/menu.cpp
    ImGuiFileDialog/ImGuiFileDialog.cpp
    ${imgui_src}
)

target_include_directories(Childhood-Fun PRIVATE
    ${DIST}/include
    src/imgui/
)

target_link_directories(Childhood-Fun PRIVATE
    ${DIST}/lib
)

target_link_libraries(Childhood-Fun PRIVATE
    MesenRT
    glfw
    OpenGL
    dl
)


install(TARGETS Childhood-Fun DESTINATION bin)
