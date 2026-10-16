cmake_minimum_required(VERSION 4.4)
project(chip_8)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
  bin/main.cpp
  bin/interpreter.cpp
  bin/opcode.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/WX>
)
