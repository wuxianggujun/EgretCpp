cmake_minimum_required(VERSION 3.31)
project(02-EventsDemo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(EGRET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
    message(STATUS "Building 02-EventsDemo as part of main project")
else()
    message(STATUS "Building 02-EventsDemo independently - including EgretEngine")
    add_subdirectory(${EGRET_ROOT} ${CMAKE_BINARY_DIR}/EgretEngine EXCLUDE_FROM_ALL)
endif()

add_executable(02-events-demo main.cpp)
target_link_libraries(02-events-demo PRIVATE EgretEngine)
target_include_directories(02-events-demo PRIVATE ${EGRET_ROOT}/src)

