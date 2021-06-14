cmake_minimum_required(VERSION 3.10)

set(SOURCE_FILES
    src/lib/emulator.cpp
)

project(chip_8)

add_subdirectory(minifb)


add_executable(emulator src/example.cpp ${SOURCE_FILES})

include_directories(src/)
include_directories(minifb/include)

target_link_libraries(emulator minifb)