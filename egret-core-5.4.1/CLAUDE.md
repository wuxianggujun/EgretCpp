# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is **Egret Engine 5.4.1**, a HTML5 game engine for 2D/3D game development written in TypeScript. The engine provides modules for rendering, GUI systems, audio, resource management, and cross-platform deployment.

## Key Commands

### Core Development Commands
- `egret build` - Build the project (compiles TypeScript to JavaScript)
- `egret startserver` - Start development server for testing
- `egret create <ProjectName>` - Create new Egret project
- `egret create <ProjectName> --type=<empty|game|gui|eui>` - Create project with specific template

### Project Management
- `egret publish` - Publish project for distribution
- `egret run` - Run the project
- `egret help` - Display help information

## Architecture Overview

### Core Structure
- **`src/egret/`** - Core engine modules:
  - `display/` - Display objects, containers, bitmap handling
  - `events/` - Event system and dispatching
  - `geom/` - Geometric utilities (Point, Rectangle, Matrix, etc.)
  - `net/` - Network communication (HTTP requests, image loading)
  - `utils/` - Core utilities and helpers
  - `web/` - Web platform-specific implementations (WebGL, Canvas, touch handling)

- **`src/extension/`** - Extension modules:
  - `eui/` - UI component library
  - `game/` - Game-specific utilities
  - `tween/` - Animation tweening library
  - `socket/` - WebSocket communication
  - `assetsmanager/` - Resource management

- **`tools/`** - Build system and CLI tools written in Node.js
  - `Entry.js` - Main entry point for CLI commands
  - `actions/` - Command implementations (compile, create, etc.)
  - `templates/` - Project templates

### Module System
The engine uses a custom module system defined in `package.json`:
- **Core module**: `egret` - Base engine functionality
- **Extensions**: `eui`, `game`, `tween`, `socket`, etc.
- Dependencies are managed between modules (e.g., eui depends on egret)

### Platform Support
- **Web platforms**: Canvas and WebGL rendering
- **Mobile**: iOS 8.0+, Android 4.0+
- **Desktop**: Chrome, Safari, Firefox, Edge, IE9+

## TypeScript Configuration

The project uses TypeScript with these key settings:
- Target: ES5
- Module: CommonJS  
- Experimental decorators enabled
- Namespace-based architecture (uses `namespace egret`)

## Important Implementation Details

### Display System
- Based on display tree similar to Flash/ActionScript 3
- Core classes: `DisplayObject`, `DisplayObjectContainer`, `Bitmap`, `Shape`
- Uses both Canvas 2D and WebGL rendering backends

### Event System
- Custom event system with `Event` and `EventDispatcher` classes
- Touch/mouse event handling abstracted across platforms
- Event bubbling and capturing phases

### Resource Management
- Asset loading and management through `assetsmanager` extension
- Support for images, audio, JSON, and other game assets
- Resource groups and loading queues

### Build Process
- Custom build system (not webpack/rollup/grunt)
- TypeScript compilation handled by internal compiler
- Module concatenation and platform-specific builds
- Debug vs release configurations in `Defines.debug.ts` and `Defines.release.ts`

## Development Workflow

1. **Project Creation**: Use `egret create` with appropriate template
2. **Development**: Edit TypeScript files in `src/`
3. **Build**: Run `egret build` to compile
4. **Test**: Use `egret startserver` to run locally
5. **Debug**: Use browser developer tools (Chrome recommended)

## Code Conventions

- Uses namespace syntax: `namespace egret { ... }`
- Class names use PascalCase
- Properties and methods use camelCase
- Extensive use of TypeScript interfaces
- Header comment blocks with BSD license in all files
- Platform-specific code separated by file paths (e.g., `web/`, `native/`)