cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(Mesen2 VERSION 0.1.0 LANGUAGES CXX C)

include(GenerateExportHeader)

# MesenLib
add_library(MesenLib STATIC "")

file(GLOB_RECURSE source 
    Core/*.cpp
    Lua/*.c 
    Utilities/*.cpp
    Utilities/*.c
    # Sdl/*.cpp
    SevenZip/*.c
    # Linux/*.cpp
    SoftwareDevice/*.cpp
)

target_sources(MesenLib PRIVATE ${source})
target_include_directories(MesenLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities
)

set_target_properties(MesenLib PROPERTIES 
    CXX_STANDARD 17
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
)

# MesenRT
add_library(MesenRT SHARED "")

set_target_properties(MesenRT PROPERTIES 
    CXX_STANDARD 17
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
)

generate_export_header(MesenRT
    EXPORT_MACRO_NAME MESEN_API
    EXPORT_FILE_NAME ExportAPI/include/Mesen/api.h
)

# file(GLOB_RECURSE export_src 
#     ExportAPI/src/*.cpp
# )

target_sources(MesenRT PRIVATE
    ExportAPI/src/dump.cpp
    ExportAPI/src/emu_api.cpp
    ExportAPI/src/config_api.cpp
    ExportAPI/src/input_api.cpp
)

target_include_directories(MesenRT PUBLIC 
    ExportAPI/include
    ${CMAKE_CURRENT_BINARY_DIR}/ExportAPI/include
)

target_link_libraries(MesenRT PRIVATE 
    MesenLib
)

target_compile_definitions(MesenRT PRIVATE USE_CMAKE ONLY_SOFTWARE_DEVICE)

# Install MesenRT

install(TARGETS MesenRT DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ExportAPI/include/Mesen DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ExportAPI/include/Mesen DESTINATION include)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    install(TARGETS MesenLib DESTINATION lib)

    target_compile_options(MesenLib PRIVATE 
        "-sSHARED_MEMORY=1"
        "-sWASM_WORKERS=1"
    )

    target_compile_options(MesenRT PRIVATE 
        "-sSHARED_MEMORY=1"
        "-sWASM_WORKERS=1"
    )
endif()

option(BUILD_TerminalPlayer "BUILD_TerminalPlayer" OFF)

if(BUILD_TerminalPlayer)
    # TerminalPlayer
    find_package(OpenCV REQUIRED)
    add_executable(TerminalPlayer ExportAPI/examples/TerminalPlayer/main.cpp)
    target_include_directories(TerminalPlayer PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(TerminalPlayer PRIVATE MesenRT ${OpenCV_LIBS} ncurses)
endif()