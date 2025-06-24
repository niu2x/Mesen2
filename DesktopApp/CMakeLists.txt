cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(DesktopApp VERSION 0.1.0 LANGUAGES CXX C)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(Childhood-Fun "")

target_sources(Childhood-Fun PRIVATE
    src/main.cpp
)

target_include_directories(Childhood-Fun PRIVATE
    ${DIST}/include
)

target_link_directories(Childhood-Fun PRIVATE
    ${DIST}/lib
)

target_link_libraries(Childhood-Fun PRIVATE
    MesenRT
    Qt5::Widgets
)


install(TARGETS Childhood-Fun DESTINATION bin)
