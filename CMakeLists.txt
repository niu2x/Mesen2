cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(Mesen2 VERSION 0.1.0 LANGUAGES C CXX)

include(GenerateExportHeader)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gen_src/embed_NES_DB.inc
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/ExportAPI/tools/generate_NES_DB.py 
                    ${CMAKE_CURRENT_SOURCE_DIR}/UI/Dependencies/MesenNesDB.txt
                    ${CMAKE_CURRENT_BINARY_DIR}/gen_src/embed_NES_DB.inc
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ExportAPI/tools/generate_NES_DB.py
    COMMENT "Generating embed_NES_DB.inc"
)

add_custom_target(embed_NES_DB
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gen_src/embed_NES_DB.inc
)


# MesenLib
add_library(MesenLib STATIC "")
# target_compile_options(MesenLib PRIVATE -ftime-trace)
target_precompile_headers(MesenLib PRIVATE Core/pch.h)

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

target_include_directories(MesenLib PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/gen_src
)

set_target_properties(MesenLib PROPERTIES 
    CXX_STANDARD 17
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    POSITION_INDEPENDENT_CODE ON
)

# target_precompile_headers(MesenLib PRIVATE Core/pch.h)

add_dependencies(MesenLib embed_NES_DB)


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
    ExportAPI/src/static_check.cpp
)

target_include_directories(MesenRT PUBLIC 
    ExportAPI/include
    ${CMAKE_CURRENT_BINARY_DIR}/ExportAPI/include
)

target_link_libraries(MesenRT PRIVATE 
    MesenLib
)
target_compile_definitions(MesenLib PRIVATE EMBED_NES_DB)
target_compile_definitions(MesenRT PRIVATE USE_CMAKE_EXPORT_HEADER ONLY_SOFTWARE_DEVICE)

# Install MesenRT

install(TARGETS MesenRT DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ExportAPI/include/Mesen DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ExportAPI/include/Mesen DESTINATION include)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    install(TARGETS MesenLib DESTINATION lib)

    target_compile_options(MesenLib PRIVATE 
        "-sUSE_PTHREADS=1"
    )
    target_compile_options(MesenRT PRIVATE 
        "-sUSE_PTHREADS=1"
    )
endif()

option(BUILD_Examples "BUILD_Examples" OFF)
option(BUILD_profiler "BUILD_profiler" OFF)

if(BUILD_Examples)
    add_executable(TakeScreenshot ExportAPI/examples/TakeScreenshot/main.cpp)
    target_link_libraries(TakeScreenshot PRIVATE MesenRT pthread)

endif()

if(BUILD_profiler)
    # profile
    add_executable(profile ExportAPI/examples/profile.cpp)
    target_link_libraries(profile PRIVATE MesenRT pthread)
endif()