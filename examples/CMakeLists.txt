# EgretCpp Examples
cmake_minimum_required(VERSION 3.31)
project(EgretCppExamples LANGUAGES C CXX)

message(STATUS "Building EgretCpp Examples")

# Add all example subdirectories
add_subdirectory(01-basic-window)
add_subdirectory(02-events-demo)
add_subdirectory(03-event-flow)
