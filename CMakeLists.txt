cmake_minimum_required(VERSION 3.31)
project(EgretCpp)

set(CMAKE_CXX_STANDARD 20)

# Set MSVC runtime library policy (for MSVC compatibility)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

# Configure runtime library based on build type
if(MSVC)
    # Match Skia's actual runtime library settings
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")  # MDd to match Skia Debug
        # Debug mode: use iterator debug level 2
        add_compile_definitions(_ITERATOR_DEBUG_LEVEL=2)
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")        # MD for Release (dynamic) to match Skia
        # Release mode: must use iterator debug level 0
        add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
    endif()
    
    # 设置源文件编码为UTF-8，避免中文注释乱码
    add_compile_options(/utf-8)
    
    # 禁用严格字符串转换检查，兼容stb_image
    add_compile_options(/Zc:strictStrings-)
endif()

# Disable tests and examples for third-party libraries
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)

# SDL3 will be configured in FindSDL.cmake
# Tests and examples are disabled there

# Include library configurations
include(cmake/FindSkia.cmake)
include(cmake/FindGLM.cmake)
include(cmake/FindSDL.cmake)

# Egret Engine Core模块源文件
set(EGRET_CORE_SOURCES
    # Core模块
    src/core/HashObject.cpp
    
    # Events模块
    src/events/Event.cpp
    src/events/EventDispatcher.cpp
    src/events/TouchEvent.cpp
    src/events/KeyboardEvent.cpp
    src/events/TimerEvent.cpp
    src/events/TextEvent.cpp
    src/events/IOErrorEvent.cpp
    src/events/ProgressEvent.cpp
    
    # Geom模块
    src/geom/Point.cpp
    src/geom/Rectangle.cpp
    src/geom/Matrix.cpp
    
    # Display模块
    src/display/DisplayObject.cpp
    src/display/DisplayObjectContainer.cpp
    src/display/DisplayList.cpp
    src/display/Stage.cpp
    src/display/Bitmap.cpp
    src/display/Texture.cpp
    src/display/BitmapData.cpp
    src/display/Graphics.cpp
    src/display/Shape.cpp
    src/display/Sprite.cpp
    src/display/RenderTexture.cpp
    src/display/SpriteSheet.cpp
    src/display/BlendMode.cpp
    src/display/LineStyles.cpp
    src/display/BitmapFillMode.cpp
    
    # Sys模块
    src/sys/Screen.cpp
    src/sys/Path2D.cpp
    src/sys/StrokePath.cpp
    src/sys/GraphicsNode.cpp
    
    # Player模块
    src/player/Player.cpp
    src/player/PlayerFactory.cpp
    src/player/RenderBuffer.cpp
    src/player/RenderNode.cpp
    src/player/NormalBitmapNode.cpp
    src/player/SystemRenderer.cpp
    src/player/SystemTicker.cpp
    src/player/SimpleFPSDisplay.cpp
    src/player/SkiaRenderer.cpp
    src/player/SkiaRenderBuffer.cpp
    src/player/nodes/TextNode.cpp
    src/player/nodes/BitmapNode.cpp
    src/player/nodes/GroupNode.cpp
    src/player/nodes/MeshNode.cpp
    
    # Platform模块
    src/platform/sdl/SDLEventConverter.cpp
    src/platform/sdl/SDLPlatform.cpp
    src/platform/sdl/SDLWindow.cpp
    
    # Utils模块
    src/utils/CallLater.cpp
    src/utils/Lifecycle.cpp
    src/utils/Timer.cpp
    src/utils/Logger.cpp
    
    # Net模块
    src/net/ImageLoader.cpp
    
    # Text模块
    src/text/HorizontalAlign.cpp
    src/text/VerticalAlign.cpp
    src/text/TextFieldType.cpp
    src/text/TextFieldInputType.cpp
    src/text/TextField.cpp
    
    # Filters模块
    src/filters/Filter.cpp
    src/filters/ColorMatrixFilter.cpp
        
)

set(EGRET_CORE_HEADERS
    # Core模块
    src/core/HashObject.hpp
    
    # Events模块
    src/events/Event.hpp
    src/events/IEventDispatcher.hpp
    src/events/EventPhase.hpp
    src/events/EventDispatcher.hpp
    src/events/TouchEvent.hpp
    src/events/KeyboardEvent.hpp
    src/events/TimerEvent.hpp
    src/events/TextEvent.hpp
    src/events/IOErrorEvent.hpp
    src/events/ProgressEvent.hpp
    
    # Geom模块
    src/geom/Point.hpp
    src/geom/Rectangle.hpp
    src/geom/Matrix.hpp
    
    # Display模块
    src/display/DisplayObject.hpp
    src/display/DisplayObjectContainer.hpp
    src/display/DisplayList.hpp
    src/display/Stage.hpp
    src/display/Bitmap.hpp
    src/display/Texture.hpp
    src/display/BitmapData.hpp
    src/display/Graphics.hpp
    src/display/Shape.hpp
    src/display/Sprite.hpp
    src/display/RenderTexture.hpp
    src/display/SpriteSheet.hpp
    src/display/BlendMode.hpp
    src/display/LineStyles.hpp
    src/display/BitmapFillMode.hpp
    
    # Sys模块
    src/sys/Screen.hpp
    src/sys/Path2D.hpp
    src/sys/StrokePath.hpp
    src/sys/GraphicsNode.hpp
    src/sys/TextFormat.hpp
    
    # Player模块
    src/player/Player.hpp
    src/player/PlayerFactory.hpp
    src/player/PlayerOption.hpp
    src/player/RenderBuffer.hpp
    src/player/RenderNode.hpp
    src/player/NormalBitmapNode.hpp
    src/player/SystemRenderer.hpp
    src/player/SystemTicker.hpp
    src/player/SimpleFPSDisplay.hpp
    src/player/SkiaRenderer.hpp
    src/player/SkiaRenderBuffer.hpp
    src/player/nodes/TextNode.hpp
    src/player/nodes/BitmapNode.hpp
    src/player/nodes/GroupNode.hpp
    src/player/nodes/MeshNode.hpp
    
    # Filters模块
    src/filters/Filter.hpp
    src/filters/ColorMatrixFilter.hpp
    
    # Platform模块
    src/platform/sdl/SDLEventConverter.hpp
    src/platform/sdl/SDLPlatform.hpp
    src/platform/sdl/SDLWindow.hpp
    
    # Utils模块
    src/utils/CallLater.hpp
    src/utils/Lifecycle.hpp
    src/utils/Timer.hpp
    
    # Net模块
    src/net/ImageLoader.hpp
    
    # Text模块
    src/text/HorizontalAlign.hpp
    src/text/VerticalAlign.hpp
    src/text/TextFieldType.hpp
    src/text/TextFieldInputType.hpp
    src/text/TextField.hpp
    
    # Filters模块
    src/filters/Filter.hpp
    src/filters/ColorMatrixFilter.hpp
)

# 创建Egret引擎静态库
add_library(EgretEngine STATIC 
    ${EGRET_CORE_SOURCES}
    ${EGRET_CORE_HEADERS}
)

# 设置包含目录
target_include_directories(EgretEngine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
)

# 为引擎链接第三方依赖
target_link_libraries(EgretEngine PUBLIC
    Skia::Skia
    glm::glm
    SDL3::SDL3
)

# 主程序可执行文件
add_executable(EgretCpp 
    main.cpp
)

# 链接Egret引擎到主程序
target_link_libraries(EgretCpp PRIVATE EgretEngine)
