cmake_minimum_required(VERSION 4.4)
project(chip_8)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(raylib CONFIG QUIET)

if(NOT raylib_FOUND)
    message(STATUS "Raylib not found locally. Fetching from GitHub...")

    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        6.2
        GIT_SHALLOW    ON
    )

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(raylib)
endif()

add_executable(${PROJECT_NAME}
  bin/main.cpp
  bin/interpreter.cpp
  bin/opcode.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/WX>
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
