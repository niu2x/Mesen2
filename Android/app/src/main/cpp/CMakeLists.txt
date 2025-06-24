cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(MesenAndroidAPI VERSION 0.1.0 LANGUAGES CXX C)

add_library(MesenAndroid SHARED "")

target_sources(MesenAndroid PRIVATE
    binding.cpp
)

target_include_directories(MesenAndroid PRIVATE
    ${DIST_DIR}/include
)

target_link_directories(MesenAndroid PRIVATE
    ${DIST_DIR}/lib
)

target_link_libraries(MesenAndroid PRIVATE
    MesenRT
)

install(TARGETS MesenAndroid DESTINATION lib)

