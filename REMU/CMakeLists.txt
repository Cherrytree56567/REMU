cmake_minimum_required(VERSION 3.19)

project(REMU)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES
    Bus.cpp
    Clint.cpp
    CPU.cpp
    Device.cpp
    Machine.cpp
    main.cpp
    OpcodeHelpers.cpp
    Opcodes.cpp
    Plic.cpp
    Trap.cpp
    Uart.cpp
    Virtio.cpp
)

if(UNIX)
    target_compile_definitions(REMU PRIVATE LINUX)
endif()

add_executable(REMU ${SOURCES})
