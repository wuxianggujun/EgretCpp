cmake_minimum_required(VERSION 3.31)
project(BasicWindow LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get the parent directory (EgretCpp root)
get_filename_component(EGRET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# Check if we're building as part of the main project
get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
    # Building as part of main project - EgretEngine already available
    message(STATUS "Building BasicWindow as part of main project")
    
else()
    # Building independently - need to build EgretEngine first
    message(STATUS "Building BasicWindow independently - including EgretEngine")
    
    # Add the main project as subdirectory to get EgretEngine
    add_subdirectory(${EGRET_ROOT} ${CMAKE_BINARY_DIR}/EgretEngine EXCLUDE_FROM_ALL)
    
endif()

# Add executable
add_executable(basic-window main.cpp)

# Link to EgretEngine (which already has all dependencies)
target_link_libraries(basic-window PRIVATE EgretEngine)

# Include EgretCpp source directories
target_include_directories(basic-window PRIVATE ${EGRET_ROOT}/src)