# Examples

This directory contains various examples demonstrating EgretCpp functionality.

## Examples List

### 01-basic-window
Basic window creation and stage initialization example. Demonstrates:
- SDL3 window creation
- Stage initialization
- Basic rendering loop
- Event handling setup

## Building Examples

Each example can be built independently:

```bash
cd examples/01-basic-window
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/basic-window
```

## Prerequisites

All examples require:
- Visual Studio 2022 (MSVC compiler)
- CMake 3.31+
- The third_party libraries from the main project