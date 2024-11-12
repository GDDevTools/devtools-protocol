cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
#set(GEODE_DISABLE_PRECOMPILED_HEADERS ON)
set(USE_ZLIB OFF)

add_compile_definitions(WIN32_LEAN_AND_MEAN)

project(Puppeteer VERSION 1.0.0)

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
message(STATUS "-------- Setting up MuJS --------")
file(GLOB_RECURSE mujs_files_h external/mujs/*.h)
add_library(mujs STATIC
    ${mujs_files_h}
    external/mujs/jsarray.c
    external/mujs/jsboolean.c
    external/mujs/jsbuiltin.c
    external/mujs/jscompile.c
    external/mujs/jsdate.c
    external/mujs/jsdtoa.c
    external/mujs/jserror.c
    external/mujs/jsfunction.c
    external/mujs/jsgc.c
    external/mujs/jsintern.c
    external/mujs/jslex.c
    external/mujs/jsmath.c
    external/mujs/jsnumber.c
    external/mujs/jsobject.c
    external/mujs/json.c
    external/mujs/jsparse.c
    external/mujs/jsproperty.c
    external/mujs/jsregexp.c
    external/mujs/jsrepr.c
    external/mujs/jsrun.c
    external/mujs/jsstate.c
    external/mujs/jsstring.c
    external/mujs/jsvalue.c
    external/mujs/regexp.c
    external/mujs/utf.c
)
target_link_libraries(${PROJECT_NAME} mujs)

message(STATUS "-------- Setting up Geode --------")
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

CPMAddPackage("gh:machinezone/IXWebSocket#c5a02f1")
target_link_libraries(${PROJECT_NAME} ixwebsocket)

# idk what this does
target_include_directories(${PROJECT_NAME} PRIVATE src)

