cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(WebAssembly VERSION 0.1.0 LANGUAGES CXX C)

add_executable(WebNative "")
target_sources(WebNative PRIVATE src/native.cpp)

target_include_directories(WebNative PRIVATE
    ${DIST}/include
)

target_link_directories(WebNative PRIVATE
    ${DIST}/lib
)

target_link_libraries(WebNative PRIVATE
    MesenRT
    MesenLib
)

target_compile_options(WebNative PRIVATE "-mbulk-memory" "-matomics")

target_link_options(WebNative PRIVATE
    "-pthread"
    "-sINITIAL_MEMORY=128MB"
    "-sMAXIMUM_MEMORY=512MB"
    "-sALLOW_MEMORY_GROWTH=1"
    # "-sATOMICS=1"
    # "-sBULK_MEMORY=1"
)