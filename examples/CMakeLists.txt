# EgretCpp Examples
cmake_minimum_required(VERSION 3.31)
project(EgretCppExamples LANGUAGES C CXX)

message(STATUS "Building EgretCpp Examples")

# Add all example subdirectories
add_subdirectory(01-basic-window)
add_subdirectory(02-events-demo)
add_subdirectory(03-event-flow)
add_subdirectory(04-hit-test)
add_subdirectory(05-keyboard-shortcuts)
add_subdirectory(06-resize-scalemode)
