cmake_minimum_required(VERSION 3.27)
project(nesse)

set(CMAKE_CXX_STANDARD 20)

add_executable(nesse main.cpp
        Source/System/NES/CPU/CPU.cpp
        Source/System/NES/Memory/Memory.cpp
        Source/System/NES/IOBus/IOBus.cpp
        Source/System/NES/APU/APU.cpp
        Source/System/NES/PPU/PPU.cpp
        Source/System/NES/NES.cpp
        Source/System/NES/ROMLoader/NES/NESLoader.cpp
        Source/System/NES/ROMLoader/NES/NESFile.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()
include_directories(Source)
