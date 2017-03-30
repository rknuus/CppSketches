cmake_minimum_required(VERSION 2.6)
project(FrameParser)
add_executable(FrameParser FrameParser.cpp)
set_target_properties(FrameParser PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES)