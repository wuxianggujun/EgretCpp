# Basic Window Example

This example demonstrates the fundamental initialization and rendering loop of EgretCpp, closely following the TypeScript version's architecture.

## What This Example Shows

### 1. Engine Initialization
- **SDL3 Window Creation**: Cross-platform window management
- **OpenGL Context Setup**: Hardware-accelerated rendering context
- **Skia Integration**: High-performance 2D graphics rendering
- **EgretCpp Component Initialization**: Stage, Screen, Player, SystemTicker

### 2. Egret Architecture Implementation
Based on the TypeScript `runEgret()` and `WebPlayer` initialization:

```cpp
// 1. Screen Management (WebPlayer equivalent)
m_screen = std::make_shared<egret::sys::Screen>();
m_screen->setDesignSize(WINDOW_WIDTH, WINDOW_HEIGHT);

// 2. Stage Creation (new egret.Stage())
m_stage = std::make_shared<egret::Stage>();
m_stage->setScreen(m_screen);
m_stage->setFrameRate(60);

// 3. SystemTicker (egret.ticker)
m_ticker = egret::sys::SystemTicker::getTicker();
m_ticker->setFrameRate(60);

// 4. Player (egret.sys.Player)
m_player = std::make_shared<egret::sys::Player>();
m_player->setStage(m_stage);
```

### 3. Rendering Pipeline
- **Skia Surface Management**: GPU-accelerated 2D rendering
- **Stage Rendering**: EgretCpp display object rendering
- **Example Content**: Animated graphics and text
- **VSync Support**: Smooth 60fps rendering

## Building

```bash
# Navigate to example directory
cd examples/01-basic-window

# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Run
./build/basic-window
```

## Controls

- **ESC**: Exit application
- **Window Resize**: Automatic stage size adjustment

## What You'll See

- A gray window with welcome text
- Engine status information
- An animated red circle
- Real-time stage dimensions and framerate display

## Key Concepts Demonstrated

### TypeScript to C++ Translation
This example shows how TypeScript Egret initialization translates to C++:

| TypeScript | C++ Equivalent |
|------------|----------------|
| `runEgret(options)` | `BasicWindowExample::initialize()` |
| `new WebPlayer(container, options)` | SDL3 + Skia setup |
| `new egret.Stage()` | `std::make_shared<egret::Stage>()` |
| `egret.sys.Player` | `egret::sys::Player` |
| `ticker.startTick()` | `SystemTicker::startTick()` |

### Architecture Patterns
- **RAII Resource Management**: Automatic cleanup in destructors
- **Smart Pointers**: Memory-safe object lifecycle management
- **Component Separation**: Clear separation of concerns (Window, Graphics, Engine)
- **Event-Driven Architecture**: SDL events driving the application loop

## Next Steps

This basic example provides the foundation for:
- Adding display objects (Sprite, Shape, Bitmap)
- Implementing touch/mouse input handling
- Creating game loops and animations
- Building complex UI layouts
- Integrating audio and assets

## Performance Notes

- **VSync Enabled**: Smooth 60fps rendering
- **GPU Acceleration**: Hardware-accelerated Skia rendering
- **Efficient Updates**: Only renders when needed
- **Memory Management**: RAII ensures proper cleanup