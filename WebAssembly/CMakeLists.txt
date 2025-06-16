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

target_compile_options(WebNative PRIVATE 
    "-sSHARED_MEMORY=1"
    "-sWASM_WORKERS=1"
)

target_link_options(WebNative PRIVATE
    "-sUSE_PTHREADS=1"
    "-sINITIAL_MEMORY=128MB"
    "-sMAXIMUM_MEMORY=512MB"
    "-sPTHREAD_POOL_SIZE=16"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sSHARED_MEMORY=1"
    "-sWASM_WORKERS=1"
)