# EgretCpp

A high-performance C++ port of Egret Engine 5.4.1, translating the complete TypeScript-based HTML5 game engine to native C++ while maintaining API compatibility and enhancing performance.

## 🎯 Project Overview

**EgretCpp** aims to bring the power of Egret Engine to native C++ development by:

- **🔄 API Compatibility** - Maintaining the same API design and behavior patterns as the TypeScript version
- **⚡ Performance Optimization** - Leveraging C++ native performance with modern math libraries (GLM) 
- **🌐 Cross-platform Support** - Built on Skia, SDL3 and other cross-platform libraries
- **📦 Modular Design** - Following the original Egret Engine's modular structure

## 🛠️ Technology Stack

### Core Dependencies
- **[Skia](https://skia.org/)** - Google's 2D graphics rendering library for high-performance graphics
- **[SDL3](https://www.libsdl.org/)** - Cross-platform multimedia and input handling
- **[GLM](https://github.com/g-truc/glm)** - OpenGL Mathematics library for optimized math operations

### Development Environment
- **Compiler**: MSVC (Visual Studio 2022)
- **Build System**: CMake 3.31+
- **C++ Standard**: C++20
- **Platform**: Windows x64 (with plans for cross-platform expansion)

## 🚀 Features

### ✅ Implemented Modules

#### Core Foundation
- **Build System** - Complete CMake configuration with Debug/Release support
- **Third-party Integration** - Seamless integration of Skia, SDL3, and GLM libraries
- **HashObject** - Base hash object class with unique ID functionality

#### Event System (Complete Implementation)
- **Event Class** - Full TypeScript-compatible event base class with object pooling
- **EventDispatcher** - Priority event system with capture/bubble support
- **TouchEvent** - Complete touch event system with coordinate transformation
- **Event Types** - Support for 20+ standard event types

#### Geometry Module (Complete Implementation)  
- **Point Class** - 2D coordinates with GLM vec2 integration and SIMD optimization
- **Rectangle Class** - Full geometric detection and transformation support
- **Matrix Class** - 2D transformation matrix with GLM mat3 optimization

#### Display System (Complete Implementation)
- **DisplayObject** - Base display object with complete coordinate transformation
- **DisplayObjectContainer** - Display tree management with deferred event handling
- **DisplayList** - Rendering pipeline with render node management
- **Stage** - Root container with frame rate control and scaling modes
- **Sprite** - Sprite display object with Graphics integration

#### System Management (Complete Implementation)
- **Screen Class** - Screen adaptation system with 6 scaling modes
- **SystemTicker** - Core game loop system with frame rate control
- **CallLater System** - Deferred and async function execution

### 🔄 Current Development Status

#### Rendering System (Latest Enhancement)
- **SystemRenderer** - Abstract renderer interface (newly refactored)
- **SkiaRenderer** - Independent Skia rendering implementation
- **RenderBuffer** - Rendering buffer management
- **RenderNode** - Rendering node system with NormalBitmapNode support

## 📁 Project Structure

```
EgretCpp/
├── src/
│   ├── core/                 # Core foundation classes
│   ├── events/               # Event system (Complete)
│   ├── geom/                 # Geometry math module (Complete)
│   ├── display/              # Display object system (Complete)
│   ├── sys/                  # System management (Complete)
│   └── player/               # Rendering system (Enhanced)
├── docs/                     # Project documentation
├── third_party/              # Third-party libraries
├── egret-core-5.4.1/         # Original Egret Engine TypeScript source
├── cmake/                    # CMake configuration files
├── CMakeLists.txt            # Main build configuration
├── main.cpp                  # Application entry point
└── CLAUDE.md                 # Development guidelines
```

## 🔧 Build Instructions

### Prerequisites
- Visual Studio 2022 (MSVC compiler)
- CMake 3.31 or higher
- Git

### Build Steps

```bash
# Clone the repository
git clone <repository-url>
cd EgretCpp

# Build the EgretCpp engine library only
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug

# Option 1: Build examples as part of main project
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_EXAMPLES=ON
cmake --build cmake-build-debug

# Option 2: Build examples independently (recommended)
cd examples/01-basic-window
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/basic-window
```

### Release Build

```bash
# Build engine library in Release mode
cmake -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release

# Build examples in Release mode
cd examples/01-basic-window
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/basic-window
```

## 🎮 Usage Example

See the complete working example in `examples/01-basic-window/`:

```cpp
// Basic EgretCpp application setup
#include "display/Stage.hpp"
#include "sys/Screen.hpp"
#include "player/Player.hpp"
#include "sys/SystemTicker.hpp"

// Create Screen (equivalent to WebPlayer screen management)
m_screen = std::make_shared<egret::sys::Screen>();
m_screen->setDesignSize(800, 600);

// Create Stage (equivalent to TypeScript new egret.Stage())
m_stage = std::make_shared<egret::Stage>();
m_stage->setScreen(m_screen);
m_stage->setFrameRate(60);

// Create SystemTicker and Player
m_ticker = egret::sys::SystemTicker::getTicker();
m_player = std::make_shared<egret::sys::Player>();

// Start the game loop
m_ticker->addPlayer(m_player);
m_ticker->startTick(callback);
```

Run the basic window example:
```bash
cd examples/01-basic-window
cmake -B build && cmake --build build
./build/basic-window
```

## 🗺️ Development Roadmap

### Phase 1: Core Event System Enhancement
- [ ] **KeyboardEvent** - Keyboard input handling
- [ ] **TimerEvent** - Timer-based events  
- [ ] **Network Events** - IOErrorEvent, ProgressEvent, HTTPStatusEvent

### Phase 2: Display Object Expansion
- [ ] **Bitmap** - Texture-based image rendering with nine-patch support
- [ ] **Shape** - Vector graphics display object
- [ ] **Graphics API** - Complete drawing API (paths, fills, strokes)

### Phase 3: Advanced Features
- [ ] **Text Rendering** - TextField and TextInput implementations
- [ ] **Animation System** - Tween library integration
- [ ] **Asset Management** - Resource loading and caching system

## 🤝 Contributing

This project follows the original Egret Engine's architecture while leveraging modern C++ best practices:

- **RAII Memory Management** - Smart pointers and automatic resource management
- **Template Programming** - Generic programming for performance optimization  
- **Modern C++20 Features** - Latest language standards for clean, efficient code
- **API Compatibility** - 100% method signature compatibility with TypeScript version

## 📄 License

This project contains the complete Egret Engine 5.4.1 source code. Please refer to the original Egret Engine license terms.

## 🔗 Related Resources

- [Original Egret Engine](https://github.com/egret-labs/egret-core)
- [Skia Graphics Library](https://skia.org/)
- [SDL3 Documentation](https://wiki.libsdl.org/SDL3/)
- [GLM Documentation](https://github.com/g-truc/glm)

## 📈 Project Status

**Current Version**: Development Phase  
**API Completion**: ~70% of core modules implemented  
**Test Coverage**: Integration testing with Skia output verification  
**Platform Support**: Windows x64 (expanding to macOS and Linux)

---

*Built with modern C++ • Powered by Skia • Cross-platform ready*