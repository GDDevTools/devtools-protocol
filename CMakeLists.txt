cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GeodeDevToolsProtocol VERSION 1.0.0)

file(GLOB_RECURSE src src/*.cpp)
file(GLOB_RECURSE incl src/*.hpp)
add_library(${PROJECT_NAME} SHARED
    ${src}
    ${incl}
    # Add any extra C++ source files here
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

CPMAddPackage("gh:machinezone/IXWebSocket#e03c0be")
target_link_libraries(${PROJECT_NAME} ixwebsocket)

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    target_compile_options(ixwebsocket PRIVATE --target=i686-pc-windows-msvc)
    target_link_options(ixwebsocket PRIVATE --target=i686-pc-windows-msvc)
endif()
