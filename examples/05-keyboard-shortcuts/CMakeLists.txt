cmake_minimum_required(VERSION 3.31)
project(05-KeyboardShortcuts LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(EGRET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
    message(STATUS "Building 05-KeyboardShortcuts as part of main project")
else()
    add_subdirectory(${EGRET_ROOT} ${CMAKE_BINARY_DIR}/EgretEngine EXCLUDE_FROM_ALL)
endif()

add_executable(05-keyboard-shortcuts main.cpp)
target_link_libraries(05-keyboard-shortcuts PRIVATE EgretEngine)
target_include_directories(05-keyboard-shortcuts PRIVATE ${EGRET_ROOT}/src)