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
message(DEBUG src)
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

message(STATUS "-------- Setting up v8 --------")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/v8)
target_link_libraries(${PROJECT_NAME} v8_compiler)
target_link_libraries(${PROJECT_NAME} v8_initializers)
# target_link_libraries(${PROJECT_NAME} v8_compiler)
target_link_libraries(${PROJECT_NAME} v8_libbase)
target_link_libraries(${PROJECT_NAME} v8_libplatform)
#target_link_libraries(${PROJECT_NAME} v8_snapshot)
#target_link_libraries(${PROJECT_NAME} v8_torque_generated)
#target_link_libraries(${PROJECT_NAME} v8-i18n-support)

message(STATUS "-------- Setting up Geode --------")

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})

CPMAddPackage("gh:machinezone/IXWebSocket#c5a02f1")
target_link_libraries(${PROJECT_NAME} ixwebsocket)

# idk what this does
target_include_directories(${PROJECT_NAME} PRIVATE src)

