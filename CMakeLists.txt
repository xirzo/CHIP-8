cmake_minimum_required(VERSION 4.4)
project(chip_8)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(chip_8
  bin/main.cpp
  bin/interpreter.cpp
  bin/opcode.cpp
)
