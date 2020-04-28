cmake_minimum_required(VERSION 3.16)
project(MIX)

set(CMAKE_CXX_STANDARD 14)

add_executable(MIX main.cpp lib/MIXWord.cpp MIX/MIXWord.h lib/MIXInstruction.cpp MIX/MIXInstruction.h lib/MIXMachine.cpp MIX/MIXMachine.h lib/MIXDevices.cpp MIX/MIXDevices.h lib/CMD.cpp MIX/CMD.h)