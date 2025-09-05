# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

EgretCpp is a C++ project that integrates three major libraries:
- **Skia** - Google's 2D graphics rendering library  
- **GLM** - OpenGL Mathematics library for 3D graphics
- **SDL3** - Cross-platform multimedia and input handling library

The project also includes **Egret Engine 5.4.1**, a complete TypeScript-based HTML5 game engine source code, in the `egret-core-5.4.1/` directory.

## Key Commands

### Build Commands

#### C++ Project (EgretCpp)
```bash
# Configure and build (Debug)
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug

# Configure and build (Release) 
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release

# Run the executable
./cmake-build-debug/EgretCpp.exe        # Debug
./cmake-build-release/EgretCpp.exe      # Release
```

#### Egret Engine (TypeScript)
```bash
cd egret-core-5.4.1/

# Core development commands
egret build                              # Build the TypeScript project
egret startserver                        # Start development server for testing
egret create <ProjectName>               # Create new Egret project
egret create <ProjectName> --type=empty  # Create with empty template
egret create <ProjectName> --type=game   # Create with game template
egret create <ProjectName> --type=gui    # Create with GUI template
egret create <ProjectName> --type=eui    # Create with EUI template

# Project management
egret publish                            # Publish project for distribution
egret run                               # Run the project
egret help                              # Display help information
```

### Clean and Rebuild
```bash
# Clean build directories
rm -rf cmake-build-*

# Full rebuild from scratch
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
```

## Project Architecture

### Core Structure
- **`main.cpp`** - Entry point demonstrating Skia graphics, GLM math, and SDL3 initialization
- **`CMakeLists.txt`** - Main build configuration
- **`cmake/`** - Custom CMake find modules for libraries:
  - `FindSkia.cmake` - Comprehensive Skia library configuration with Debug/Release support
  - `FindGLM.cmake` - GLM header-only library setup
  - `FindSDL.cmake` - SDL3 build configuration with disabled tests/examples
- **`third_party/`** - External library dependencies (Skia, GLM, SDL)
- **`src/`** - Future C++ source code (currently empty directories: `core/`, `eui/`)

### Egret Engine 5.4.1 (TypeScript Source Code)
Located in `egret-core-5.4.1/` - Complete HTML5 game engine source code:

#### Engine Core Modules (`src/egret/`)
- **`display/`** - Display objects, containers, bitmap handling, sprite management
- **`events/`** - Event system and dispatching (touch, mouse, keyboard events)
- **`geom/`** - Geometric utilities (Point, Rectangle, Matrix, Transform)
- **`net/`** - Network communication (HTTP requests, image loading, URLLoader)
- **`utils/`** - Core utilities and helpers (Timer, HashObject, NumberUtils)
- **`web/`** - Web platform implementations (WebGL, Canvas, touch handling)

#### Extension Modules (`src/extension/`)
- **`eui/`** - Enterprise UI component library (Button, Label, List, etc.)
- **`game/`** - Game-specific utilities (URLLoader, MovieClip, ScrollView)
- **`tween/`** - Animation tweening library (Tween, Ease functions)
- **`socket/`** - WebSocket communication support
- **`assetsmanager/`** - Advanced resource management system

#### Build and Development System (`tools/`)
- **`Entry.js`** - Main CLI entry point for Egret commands
- **`actions/`** - Command implementations:
  - `Build.js` - TypeScript compilation and project building
  - `Create.js` - Project template generation
  - `Publish.js` - Distribution package creation
  - `StartServer.js` - Development server implementation
- **`templates/`** - Project templates (empty, game, gui, eui)
- **`lib/`** - Build system utilities and TypeScript compiler integration

#### Key Technical Features
- **Display Tree Architecture**: Flash/ActionScript 3-like display object model
- **Event System**: Custom implementation with bubbling and capturing phases  
- **Multi-platform Rendering**: Canvas 2D and WebGL backends
- **Module System**: Custom dependency management defined in `package.json`
- **TypeScript Integration**: Full TypeScript support with namespace-based architecture
- **Cross-platform Support**: Web, iOS 8.0+, Android 4.0+, Desktop browsers

### Library Configuration Details

**Skia Integration:**
- Auto-selects Debug/Release builds based on `CMAKE_BUILD_TYPE`
- Handles complex dependency chain (harfbuzz, icu, libjpeg, libpng, etc.)
- MSVC runtime library matching to prevent linking errors
- Special iterator debug level handling: `_ITERATOR_DEBUG_LEVEL=0` for both Debug/Release

**Runtime Library Management:**
- Both Debug and Release use static runtime libraries (`MultiThreadedDebug`/`MultiThreaded`)
- This matches Skia's compiled runtime library to avoid linker errors
- See `docs/skia-runtime-library-mismatch-troubleshooting.md` for detailed troubleshooting

## Build System Details

### CMake Configuration
- **C++ Standard**: C++20
- **Platform**: Windows x64 (MSVC-specific configurations)
- **Runtime Library Strategy**: Static linking to match Skia libraries
- **Third-party Library Management**: Disabled tests and examples to speed up builds

### Library Paths
- **Skia**: `third_party/Skia/out/{Debug|Release}-windows-x64/`
- **GLM**: `third_party/glm/` (header-only)
- **SDL**: `third_party/SDL/` (built as subdirectory)

### Known Issues and Solutions
- **Skia Runtime Library Mismatch**: Libraries may be compiled with different settings than expected
- **Solution**: CMake configuration forces matching runtime library settings
- **Cache Issues**: Clear CMake cache when switching build types or after library changes

## Development Workflow

### C++ Development (EgretCpp)
1. **Setup**: Ensure all third-party libraries are properly placed in `third_party/`
2. **Configure**: Run CMake configure step for desired build type
3. **Build**: Compile the project
4. **Test**: Run executable to verify Skia graphics output (`skia_test.png`)
5. **Debug**: Use Visual Studio or compatible debugger with Debug build

### Egret Engine Development (TypeScript)
1. **Project Setup**: Navigate to `egret-core-5.4.1/` directory
2. **Development**: Edit TypeScript source files in `src/egret/` or `src/extension/`
3. **Build**: Run `egret build` to compile TypeScript to JavaScript
4. **Test**: Use `egret startserver` to run development server
5. **Debug**: Use browser developer tools (Chrome recommended)
6. **Package**: Use `egret publish` for production builds

### Working with Egret Engine Source Code
- **Entry Point**: `src/egret/egret.ts` - Main engine namespace definitions
- **Core Classes**: 
  - `DisplayObject` - Base class for all display objects
  - `EventDispatcher` - Event handling system
  - `Texture` and `RenderTexture` - Graphics resource management
- **Module Development**: Follow namespace syntax `namespace egret { ... }`
- **TypeScript Configuration**: Target ES5, uses experimental decorators
- **Testing**: Create test projects using `egret create` with engine source

## Development Guidelines

### Testing Policy
**IMPORTANT**: Do not create test files, demo programs, or example code unless explicitly requested by the user. Focus on core implementation only.

### Implementation Principles
1. **Code-Only Focus**: Implement only the requested functionality without additional testing or demo code
2. **Core Implementation**: Prioritize core functionality over examples or demonstrations
3. **User-Driven Testing**: Only create test files when the user specifically asks for them
4. **Clean Architecture**: Focus on clean, production-ready code without unnecessary scaffolding

## Important Implementation Notes

### Virtual Function Design Pattern (重要架构改进)

**问题背景**：
在DisplayObject类设计中，最初采用了混淆的Virtual后缀命名方式：
- `onRemoveFromStage()` - 内部实现方法
- `onRemoveFromStageVirtual()` - 虚函数版本

**架构改进方案**：
重构为更清晰的设计模式：

```cpp
// DisplayObject基类设计
class DisplayObject {
protected:
    // 内部实现方法（处理核心逻辑）
    void onRemoveFromStageInternal() {
        m_stage = nullptr;
        m_nestLevel = 0;
        m_hasAddToStage = false;
        // 其他核心逻辑...
    }
    
    // 公共虚函数接口（供子类重写）
    virtual void onRemoveFromStage() {
        onRemoveFromStageInternal();
    }
};

// 子类实现示例
class Shape : public DisplayObject {
protected:
    void onRemoveFromStage() override {
        DisplayObject::onRemoveFromStage();  // 调用父类虚函数
        if (m_graphics) {
            m_graphics->onRemoveFromStage();
        }
    }
};
```

**改进优势**：
1. **直观命名**：虚函数使用自然的方法名，无需Virtual后缀
2. **职责分离**：虚函数负责多态调用，内部方法负责核心逻辑
3. **架构清晰**：符合C++设计惯例和面向对象原则
4. **向后兼容**：所有现有调用保持不变

**适用范围**：
- `onAddToStage()` / `onAddToStageInternal()`
- `onRemoveFromStage()` / `onRemoveFromStageInternal()`  
- 其他需要子类扩展的生命周期方法

**实施建议**：
在设计新的虚函数时，采用此模式确保代码清晰性和一致性。

### Graphics Integration
- Skia provides software and GPU-accelerated 2D rendering
- GLM handles mathematical operations for 3D transformations  
- SDL3 manages window creation, input handling, and cross-platform compatibility

### Current Functionality
- Creates Skia raster surface (256x256)
- Draws red circle using anti-aliased painting
- Exports result as PNG file
- Demonstrates basic GLM vector operations
- Initializes SDL3 and reports version information

### Extension Points
- **`src/core/`** - Intended for core C++ game engine components
- **`src/eui/`** - Intended for UI system implementation  
- **Integration potential**: Bridge between Egret TypeScript engine and C++ native performance
- **Egret Engine Customization**: Modify engine source in `egret-core-5.4.1/src/` for custom functionality
- **Cross-Language Integration**: Potential WebAssembly compilation of C++ code for web deployment alongside Egret Engine