cmake_minimum_required(VERSION 3.17)
project(CrashSim)

set(CMAKE_CXX_STANDARD 14)

add_executable(CrashSim main.cpp Graph.h CrashSim.h Random.h)