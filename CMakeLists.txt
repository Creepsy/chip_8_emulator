cmake_minimum_required(VERSION 3.10)

set(SOURCE_FILES
    src/lib/emulator.cpp
)

project(chip_8)

add_executable(emulator src/example.cpp ${SOURCE_FILES})

include_directories(src/)