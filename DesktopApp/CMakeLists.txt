cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(DesktopApp VERSION 0.1.0 LANGUAGES CXX C)

find_package(Qt5 REQUIRED COMPONENTS Widgets Multimedia)
set(CMAKE_AUTORCC ON)

add_executable(Childhood-Fun "")

target_sources(Childhood-Fun PRIVATE
    src/main.cpp
    src/GameView.cpp
    src/GameSoundDevice.cpp
    src/MainWindow.cpp
    src/ControlsSettingDialog.cpp
    src/VirtualKey.cpp
    resources.qrc
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
    Qt5::Multimedia
    OpenGL
)


install(TARGETS Childhood-Fun DESTINATION bin)


set_target_properties(Childhood-Fun PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib"
    BUILD_WITH_INSTALL_RPATH TRUE
)

set_target_properties(Childhood-Fun PROPERTIES 
    CXX_STANDARD 17
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
)