# 白鹭引擎C++移植指南

## 概述

本文档提供将白鹭引擎5.4.1从TypeScript/JavaScript移植到C++的完整技术指南。包含架构设计、语言特性映射、性能优化策略以及具体的实现方案。

## 目录

1. [项目概览和目标](#项目概览和目标)
2. [架构设计和模块映射](#架构设计和模块映射)
3. [TypeScript到C++的语言映射](#TypeScript到C++的语言映射)
4. [核心系统设计](#核心系统设计)
5. [渲染系统移植](#渲染系统移植)
6. [资源管理系统](#资源管理系统)
7. [跨平台支持策略](#跨平台支持策略)
8. [性能优化考虑](#性能优化考虑)
9. [开发工具链](#开发工具链)
10. [实施路线图](#实施路线图)

---

## 项目概览和目标

### 移植动机

#### 性能优势
- **执行效率**: C++编译后的性能比JavaScript快5-20倍
- **内存管理**: 更精确的内存控制，减少GC停顿
- **硬件优化**: 更好的SIMD指令支持和CPU缓存利用

#### 部署优势
- **原生应用**: 无需浏览器环境，可独立运行
- **移动平台**: 更好的移动设备性能和电池寿命
- **嵌入式支持**: 可在资源受限的设备上运行

### 项目范围

#### 核心功能移植
- ✅ **显示系统**: DisplayObject层次结构和渲染
- ✅ **事件系统**: EventDispatcher和事件传播
- ✅ **资源管理**: 图片、音频、数据加载
- ✅ **动画系统**: Tween动画和MovieClip
- ✅ **输入处理**: 鼠标、键盘、触摸事件

#### 扩展功能移植
- 🔄 **物理引擎**: Box2D集成（已有C++版本）
- 🔄 **网络通信**: HTTP请求和WebSocket
- 🔄 **音频系统**: 多平台音频播放
- ⏳ **EUI系统**: 可视化UI设计工具

### 技术栈选择

#### 渲染后端
```cpp
// OpenGL/Vulkan渲染抽象
class RenderContext {
public:
    virtual ~RenderContext() = default;
    virtual void clear(const Color& color) = 0;
    virtual void drawTriangles(const VertexBuffer& vertices, 
                              const IndexBuffer& indices) = 0;
    virtual void setShader(std::shared_ptr<Shader> shader) = 0;
};

// OpenGL实现
class OpenGLRenderContext : public RenderContext {
    // OpenGL具体实现
};

// Vulkan实现  
class VulkanRenderContext : public RenderContext {
    // Vulkan具体实现
};
```

#### 跨平台框架
- **窗口管理**: GLFW (轻量) 或 SDL2 (全功能)
- **文件系统**: std::filesystem (C++17) 或 boost::filesystem
- **网络库**: libcurl (HTTP) + websocketpp (WebSocket)
- **音频库**: OpenAL (跨平台) 或 FMOD (商业)

---

## 架构设计和模块映射

### 整体架构对比

#### TypeScript原架构
```
egret/
├── display/          # 显示对象系统
├── events/           # 事件系统  
├── geom/             # 几何运算
├── net/              # 网络和资源加载
├── utils/            # 工具类
├── web/              # Web平台适配
└── extension/        # 扩展模块
    ├── eui/          # UI组件
    ├── game/         # 游戏工具
    ├── tween/        # 动画
    └── socket/       # 网络通信
```

#### C++目标架构
```
EgretEngine/
├── Core/             # 核心系统
│   ├── Display/      # 显示对象系统
│   ├── Events/       # 事件系统
│   ├── Geom/         # 几何运算
│   ├── Utils/        # 工具类
│   └── Memory/       # 内存管理
├── Rendering/        # 渲染系统
│   ├── OpenGL/       # OpenGL后端
│   ├── Vulkan/       # Vulkan后端
│   └── Software/     # 软件渲染(备用)
├── Resources/        # 资源系统
│   ├── Loaders/      # 各类加载器
│   ├── Cache/        # 缓存管理
│   └── Managers/     # 资源管理
├── Platform/         # 平台适配
│   ├── Windows/      # Windows平台
│   ├── Linux/        # Linux平台
│   ├── MacOS/        # macOS平台
│   ├── Android/      # Android平台
│   └── iOS/          # iOS平台
├── Audio/            # 音频系统
├── Network/          # 网络通信
├── Extensions/       # 扩展模块
│   ├── UI/           # UI组件系统
│   ├── Animation/    # 动画系统
│   └── Physics/      # 物理引擎
└── Tools/            # 开发工具
    ├── AssetPipeline/ # 资源管道
    └── SceneEditor/   # 场景编辑器
```

### 核心类层次结构设计

#### 显示对象系统
```cpp
// 基础显示对象
class DisplayObject : public EventTarget {
public:
    // 变换属性
    float x = 0.0f, y = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f;
    float rotation = 0.0f;
    float alpha = 1.0f;
    bool visible = true;
    
    // 层次关系
    DisplayObjectContainer* parent = nullptr;
    
    // 生命周期
    virtual void update(float deltaTime) {}
    virtual void render(RenderContext& context) = 0;
    
    // 几何运算
    virtual Rectangle getBounds() const = 0;
    virtual bool hitTest(const Point& point) const = 0;
    
    // 矩阵变换
    const Matrix& getTransform() const;
    Matrix getConcatenatedMatrix() const;
    
protected:
    mutable Matrix m_transform;
    mutable bool m_transformDirty = true;
    
    void updateTransform() const;
};

// 容器类
class DisplayObjectContainer : public DisplayObject {
public:
    // 子对象管理
    void addChild(std::shared_ptr<DisplayObject> child);
    void removeChild(std::shared_ptr<DisplayObject> child);
    void removeChildAt(size_t index);
    std::shared_ptr<DisplayObject> getChildAt(size_t index) const;
    size_t getChildIndex(std::shared_ptr<DisplayObject> child) const;
    size_t numChildren() const { return m_children.size(); }
    
    // 渲染和更新
    void update(float deltaTime) override;
    void render(RenderContext& context) override;
    
    // 碰撞检测
    Rectangle getBounds() const override;
    bool hitTest(const Point& point) const override;
    
private:
    std::vector<std::shared_ptr<DisplayObject>> m_children;
};

// 具体显示对象
class Bitmap : public DisplayObject {
public:
    explicit Bitmap(std::shared_ptr<Texture> texture);
    
    void render(RenderContext& context) override;
    Rectangle getBounds() const override;
    bool hitTest(const Point& point) const override;
    
    std::shared_ptr<Texture> texture;
    Rectangle sourceRect;  // UV坐标
    
private:
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
};
```

---

## TypeScript到C++的语言映射

### 基础类型映射

| TypeScript | C++ | 说明 |
|------------|-----|------|
| `number` | `float/double` | 浮点数运算 |
| `string` | `std::string` | 字符串处理 |
| `boolean` | `bool` | 布尔值 |
| `Array<T>` | `std::vector<T>` | 动态数组 |
| `Object/Map` | `std::unordered_map` | 关联容器 |
| `Function` | `std::function` | 函数对象 |
| `Promise<T>` | `std::future<T>` | 异步操作 |

### 面向对象特性映射

#### 类继承和多态
```typescript
// TypeScript原代码
abstract class DisplayObject extends EventTarget {
    public x: number = 0;
    public y: number = 0;
    
    abstract render(context: RenderContext): void;
    
    public getBounds(): Rectangle {
        // 实现
    }
}

class Bitmap extends DisplayObject {
    constructor(public texture: Texture) {
        super();
    }
    
    render(context: RenderContext): void {
        // 渲染实现
    }
}
```

```cpp
// C++对应代码
class DisplayObject : public EventTarget {
public:
    float x = 0.0f;
    float y = 0.0f;
    
    virtual ~DisplayObject() = default;
    virtual void render(RenderContext& context) = 0;
    
    Rectangle getBounds() const {
        // 实现
    }
};

class Bitmap : public DisplayObject {
public:
    explicit Bitmap(std::shared_ptr<Texture> texture) 
        : texture(std::move(texture)) {}
    
    void render(RenderContext& context) override {
        // 渲染实现
    }
    
    std::shared_ptr<Texture> texture;
};
```

#### 命名空间映射
```typescript
// TypeScript命名空间
namespace egret {
    export class Stage extends DisplayObjectContainer {
        // ...
    }
    
    export namespace sys {
        export class Player {
            // ...
        }
    }
}
```

```cpp
// C++命名空间
namespace Egret {
    class Stage : public DisplayObjectContainer {
        // ...
    };
    
    namespace Sys {
        class Player {
            // ...
        };
    }
}
```

### 内存管理策略

#### 智能指针使用原则
```cpp
// 所有权管理指南
class ResourceManager {
private:
    // 1. unique_ptr - 独占所有权
    std::unique_ptr<AudioContext> m_audioContext;
    
    // 2. shared_ptr - 共享所有权
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    
    // 3. weak_ptr - 弱引用，避免循环引用
    std::vector<std::weak_ptr<DisplayObject>> m_displayObjects;
    
    // 4. 原始指针 - 非所有权关系
    DisplayObject* m_currentFocus = nullptr;
};

// RAII资源管理
class TextureResource {
public:
    explicit TextureResource(const std::string& path) {
        // 加载纹理
        m_handle = loadTexture(path);
    }
    
    ~TextureResource() {
        // 自动清理
        if (m_handle != 0) {
            glDeleteTextures(1, &m_handle);
        }
    }
    
    // 禁止复制，允许移动
    TextureResource(const TextureResource&) = delete;
    TextureResource& operator=(const TextureResource&) = delete;
    
    TextureResource(TextureResource&& other) noexcept 
        : m_handle(std::exchange(other.m_handle, 0)) {}
    
    TextureResource& operator=(TextureResource&& other) noexcept {
        if (this != &other) {
            std::swap(m_handle, other.m_handle);
        }
        return *this;
    }
    
private:
    GLuint m_handle = 0;
    GLuint loadTexture(const std::string& path);
};
```

### 异步编程模式

#### Promise到Future的映射
```typescript
// TypeScript Promise
class ImageLoader extends EventTarget {
    load(url: string): Promise<BitmapData> {
        return new Promise((resolve, reject) => {
            const img = new Image();
            img.onload = () => resolve(new BitmapData(img));
            img.onerror = () => reject(new Error(`Failed to load ${url}`));
            img.src = url;
        });
    }
}
```

```cpp
// C++ Future/Promise
class ImageLoader : public EventTarget {
public:
    std::future<std::shared_ptr<BitmapData>> load(const std::string& url) {
        auto promise = std::make_shared<std::promise<std::shared_ptr<BitmapData>>>();
        auto future = promise->get_future();
        
        // 异步加载
        std::thread([this, url, promise]() {
            try {
                auto bitmapData = loadImageSync(url);
                promise->set_value(bitmapData);
                dispatchEvent(std::make_shared<Event>(Event::COMPLETE));
            } catch (const std::exception& e) {
                promise->set_exception(std::current_exception());
                dispatchEvent(std::make_shared<IOErrorEvent>(IOErrorEvent::IO_ERROR));
            }
        }).detach();
        
        return future;
    }
    
private:
    std::shared_ptr<BitmapData> loadImageSync(const std::string& url);
};
```

---

## 核心系统设计

### 事件系统

#### 事件基类设计
```cpp
// 事件基类
class Event {
public:
    static constexpr const char* COMPLETE = "complete";
    static constexpr const char* CHANGE = "change";
    static constexpr const char* RESIZE = "resize";
    
    explicit Event(const std::string& type, bool bubbles = false)
        : m_type(type), m_bubbles(bubbles) {}
    
    virtual ~Event() = default;
    
    const std::string& type() const { return m_type; }
    bool bubbles() const { return m_bubbles; }
    
    DisplayObject* target() const { return m_target; }
    DisplayObject* currentTarget() const { return m_currentTarget; }
    
    void stopPropagation() { m_propagationStopped = true; }
    void stopImmediatePropagation() { m_immediatePropagationStopped = true; }
    
    bool isPropagationStopped() const { return m_propagationStopped; }
    bool isImmediatePropagationStopped() const { return m_immediatePropagationStopped; }
    
private:
    friend class EventDispatcher;
    
    std::string m_type;
    bool m_bubbles;
    DisplayObject* m_target = nullptr;
    DisplayObject* m_currentTarget = nullptr;
    bool m_propagationStopped = false;
    bool m_immediatePropagationStopped = false;
};

// 事件监听器
using EventListener = std::function<void(const Event&)>;

// 事件分发器
class EventDispatcher {
public:
    virtual ~EventDispatcher() = default;
    
    void addEventListener(const std::string& type, EventListener listener, int priority = 0);
    void removeEventListener(const std::string& type, const EventListener& listener);
    void removeAllEventListeners(const std::string& type = "");
    
    bool hasEventListener(const std::string& type) const;
    bool dispatchEvent(std::shared_ptr<Event> event);
    
protected:
    void dispatchEventWith(const std::string& type, bool bubbles = false);
    
private:
    struct ListenerInfo {
        EventListener listener;
        int priority;
        
        bool operator<(const ListenerInfo& other) const {
            return priority > other.priority; // 高优先级在前
        }
    };
    
    std::unordered_map<std::string, std::vector<ListenerInfo>> m_listeners;
};
```

#### 触摸事件系统
```cpp
// 触摸事件
class TouchEvent : public Event {
public:
    static constexpr const char* TOUCH_BEGIN = "touchBegin";
    static constexpr const char* TOUCH_MOVE = "touchMove";
    static constexpr const char* TOUCH_END = "touchEnd";
    
    TouchEvent(const std::string& type, float stageX, float stageY, int touchId)
        : Event(type, true), m_stageX(stageX), m_stageY(stageY), m_touchId(touchId) {}
    
    float stageX() const { return m_stageX; }
    float stageY() const { return m_stageY; }
    int touchId() const { return m_touchId; }
    
    Point localPoint(DisplayObject* displayObject) const;
    
private:
    float m_stageX, m_stageY;
    int m_touchId;
};

// 触摸管理器
class TouchManager {
public:
    void processTouchEvent(const PlatformTouchEvent& platformEvent);
    void setStage(std::shared_ptr<Stage> stage) { m_stage = stage; }
    
private:
    std::weak_ptr<Stage> m_stage;
    std::unordered_map<int, Point> m_activeTouches;
    
    std::shared_ptr<DisplayObject> getTopMostTarget(const Point& stagePoint);
    void dispatchTouchEvent(std::shared_ptr<DisplayObject> target, 
                           std::shared_ptr<TouchEvent> event);
};
```

### 渲染系统架构

#### 渲染上下文抽象
```cpp
// 顶点数据结构
struct Vertex {
    Vector3 position;
    Vector2 texCoord;
    Color color;
    
    static std::vector<VertexAttribute> getAttributes() {
        return {
            {0, 3, VertexAttribute::FLOAT, false, sizeof(Vertex), offsetof(Vertex, position)},
            {1, 2, VertexAttribute::FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord)},
            {2, 4, VertexAttribute::UNSIGNED_BYTE, true, sizeof(Vertex), offsetof(Vertex, color)}
        };
    }
};

// 渲染状态
struct RenderState {
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    BlendMode blendMode = BlendMode::NORMAL;
    Matrix transform;
    Rectangle clipRect;
    float alpha = 1.0f;
};

// 渲染命令
class RenderCommand {
public:
    virtual ~RenderCommand() = default;
    virtual void execute(RenderContext& context) = 0;
    
    int priority = 0;
    RenderState state;
};

class DrawTrianglesCommand : public RenderCommand {
public:
    DrawTrianglesCommand(std::shared_ptr<VertexBuffer> vertices,
                        std::shared_ptr<IndexBuffer> indices)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {}
    
    void execute(RenderContext& context) override {
        context.drawTriangles(*m_vertices, *m_indices);
    }
    
private:
    std::shared_ptr<VertexBuffer> m_vertices;
    std::shared_ptr<IndexBuffer> m_indices;
};
```

#### 渲染队列和批处理
```cpp
// 渲染队列
class RenderQueue {
public:
    void addCommand(std::unique_ptr<RenderCommand> command);
    void sort(); // 按优先级和状态排序
    void execute(RenderContext& context);
    void clear();
    
private:
    std::vector<std::unique_ptr<RenderCommand>> m_commands;
    
    // 状态变化最小化排序
    void sortByState();
};

// 批处理管理器
class BatchRenderer {
public:
    void beginBatch();
    void addQuad(const Vertex vertices[4], std::shared_ptr<Texture> texture);
    void flush(RenderContext& context);
    
private:
    static constexpr size_t MAX_BATCH_SIZE = 1000;
    
    std::vector<Vertex> m_vertices;
    std::vector<uint16_t> m_indices;
    std::shared_ptr<Texture> m_currentTexture;
    size_t m_quadCount = 0;
    
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
};
```

---

## 资源管理系统

### 资源抽象和管理

#### 资源基类设计
```cpp
// 资源基类
class Resource {
public:
    enum class State {
        UNLOADED,
        LOADING,
        LOADED,
        ERROR
    };
    
    virtual ~Resource() = default;
    
    const std::string& path() const { return m_path; }
    State state() const { return m_state; }
    const std::string& error() const { return m_error; }
    
    virtual size_t getMemoryUsage() const = 0;
    virtual void reload() = 0;
    virtual void unload() = 0;
    
protected:
    explicit Resource(std::string path) : m_path(std::move(path)) {}
    
    void setState(State state) { m_state = state; }
    void setError(const std::string& error) { 
        m_error = error; 
        m_state = State::ERROR; 
    }
    
private:
    std::string m_path;
    State m_state = State::UNLOADED;
    std::string m_error;
};

// 纹理资源
class Texture : public Resource {
public:
    explicit Texture(const std::string& path);
    ~Texture();
    
    int width() const { return m_width; }
    int height() const { return m_height; }
    GLuint handle() const { return m_handle; }
    
    size_t getMemoryUsage() const override;
    void reload() override;
    void unload() override;
    
    // 异步加载
    std::future<bool> loadAsync();
    
private:
    int m_width = 0, m_height = 0;
    int m_channels = 0;
    GLuint m_handle = 0;
    
    bool loadFromFile();
    void uploadToGPU(const unsigned char* data);
};
```

#### 资源管理器
```cpp
// 资源管理器
class ResourceManager : public Singleton<ResourceManager> {
public:
    // 资源获取（自动加载）
    template<typename T>
    std::shared_ptr<T> getResource(const std::string& path) {
        auto it = m_resources.find(path);
        if (it != m_resources.end()) {
            return std::dynamic_pointer_cast<T>(it->second);
        }
        
        auto resource = std::make_shared<T>(path);
        m_resources[path] = resource;
        
        // 异步加载
        if constexpr (std::is_base_of_v<Resource, T>) {
            resource->loadAsync();
        }
        
        return resource;
    }
    
    // 预加载资源组
    std::future<void> preloadGroup(const std::string& groupName);
    
    // 内存管理
    void unloadUnusedResources();
    void unloadAll();
    size_t getTotalMemoryUsage() const;
    
    // 资源组管理
    void defineGroup(const std::string& name, const std::vector<std::string>& paths);
    void loadGroup(const std::string& name);
    void unloadGroup(const std::string& name);
    
private:
    std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources;
    std::unordered_map<std::string, std::vector<std::string>> m_groups;
    
    mutable std::shared_mutex m_mutex;
    
    // LRU缓存管理
    struct ResourceInfo {
        std::shared_ptr<Resource> resource;
        std::chrono::steady_clock::time_point lastAccessed;
        int referenceCount;
    };
    
    void updateAccessTime(const std::string& path);
    void cleanupUnusedResources();
};
```

### 资源加载器系统

#### 加载器抽象
```cpp
// 资源加载器接口
template<typename ResourceType>
class ResourceLoader {
public:
    virtual ~ResourceLoader() = default;
    
    virtual bool canLoad(const std::string& extension) const = 0;
    virtual std::unique_ptr<ResourceType> load(const std::string& path) = 0;
    virtual std::future<std::unique_ptr<ResourceType>> loadAsync(const std::string& path) = 0;
};

// 图片加载器
class ImageLoader : public ResourceLoader<Texture> {
public:
    bool canLoad(const std::string& extension) const override {
        return extension == ".png" || extension == ".jpg" || 
               extension == ".jpeg" || extension == ".bmp";
    }
    
    std::unique_ptr<Texture> load(const std::string& path) override;
    std::future<std::unique_ptr<Texture>> loadAsync(const std::string& path) override;
    
private:
    std::unique_ptr<Texture> loadFromMemory(const std::vector<uint8_t>& data, 
                                           const std::string& path);
};

// 加载器工厂
class LoaderFactory : public Singleton<LoaderFactory> {
public:
    template<typename LoaderType, typename ResourceType>
    void registerLoader() {
        auto loader = std::make_unique<LoaderType>();
        m_loaders[typeid(ResourceType)] = std::move(loader);
    }
    
    template<typename ResourceType>
    ResourceLoader<ResourceType>* getLoader() const {
        auto it = m_loaders.find(typeid(ResourceType));
        return it != m_loaders.end() ? 
            static_cast<ResourceLoader<ResourceType>*>(it->second.get()) : nullptr;
    }
    
private:
    std::unordered_map<std::type_index, std::unique_ptr<void*>> m_loaders;
};
```

---

## 跨平台支持策略

### 平台抽象层设计

#### 窗口系统抽象
```cpp
// 窗口抽象接口
class Window {
public:
    virtual ~Window() = default;
    
    virtual bool create(int width, int height, const std::string& title) = 0;
    virtual void destroy() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    
    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;
    
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual bool shouldClose() const = 0;
    
    // 事件回调
    std::function<void(int, int)> onResize;
    std::function<void(int, int)> onMouseMove;
    std::function<void(int, int, int)> onMouseButton;
    std::function<void(int, int)> onKeyboard;
    std::function<void()> onClose;
    
protected:
    int m_width, m_height;
    std::string m_title;
};

// GLFW实现
class GLFWWindow : public Window {
public:
    bool create(int width, int height, const std::string& title) override;
    void destroy() override;
    void show() override { glfwShowWindow(m_window); }
    void hide() override { glfwHideWindow(m_window); }
    
    void swapBuffers() override { glfwSwapBuffers(m_window); }
    void pollEvents() override { glfwPollEvents(); }
    
    int width() const override { return m_width; }
    int height() const override { return m_height; }
    bool shouldClose() const override { return glfwWindowShouldClose(m_window); }
    
private:
    GLFWwindow* m_window = nullptr;
    
    static void resizeCallback(GLFWwindow* window, int width, int height);
    static void mouseMoveCallback(GLFWwindow* window, double x, double y);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
```

#### 文件系统抽象
```cpp
// 文件系统接口
class FileSystem {
public:
    virtual ~FileSystem() = default;
    
    virtual bool exists(const std::string& path) const = 0;
    virtual std::vector<uint8_t> readFile(const std::string& path) const = 0;
    virtual bool writeFile(const std::string& path, const std::vector<uint8_t>& data) = 0;
    virtual std::vector<std::string> listDirectory(const std::string& path) const = 0;
    
    virtual std::string getExecutablePath() const = 0;
    virtual std::string getDocumentsPath() const = 0;
    virtual std::string getTempPath() const = 0;
};

// 标准文件系统实现
class StandardFileSystem : public FileSystem {
public:
    bool exists(const std::string& path) const override {
        return std::filesystem::exists(path);
    }
    
    std::vector<uint8_t> readFile(const std::string& path) const override {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> data(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
        
        return data;
    }
    
    bool writeFile(const std::string& path, const std::vector<uint8_t>& data) override {
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        return file.good();
    }
    
    // 平台特定实现
    std::string getExecutablePath() const override;
    std::string getDocumentsPath() const override;
    std::string getTempPath() const override;
};

// Android Assets文件系统
#ifdef ANDROID
class AndroidAssetFileSystem : public FileSystem {
public:
    explicit AndroidAssetFileSystem(AAssetManager* assetManager)
        : m_assetManager(assetManager) {}
    
    bool exists(const std::string& path) const override {
        AAsset* asset = AAssetManager_open(m_assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
        if (asset) {
            AAsset_close(asset);
            return true;
        }
        return false;
    }
    
    std::vector<uint8_t> readFile(const std::string& path) const override {
        AAsset* asset = AAssetManager_open(m_assetManager, path.c_str(), AASSET_MODE_STREAMING);
        if (!asset) {
            throw std::runtime_error("Cannot open asset: " + path);
        }
        
        size_t size = AAsset_getLength(asset);
        std::vector<uint8_t> data(size);
        AAsset_read(asset, data.data(), size);
        AAsset_close(asset);
        
        return data;
    }
    
private:
    AAssetManager* m_assetManager;
};
#endif
```

### 平台特定实现

#### 构建系统配置
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(EgretEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 平台检测
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    if(IOS)
        set(PLATFORM_IOS TRUE)
    else()
        set(PLATFORM_MACOS TRUE)
    endif()
elseif(ANDROID)
    set(PLATFORM_ANDROID TRUE)
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
endif()

# 第三方库
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

if(PLATFORM_WINDOWS)
    find_package(GLEW REQUIRED)
endif()

# 平台特定源文件
set(PLATFORM_SOURCES "")
if(PLATFORM_WINDOWS)
    list(APPEND PLATFORM_SOURCES
        src/Platform/Windows/WindowsWindow.cpp
        src/Platform/Windows/WindowsFileSystem.cpp
        src/Platform/Windows/WindowsApplication.cpp
    )
elseif(PLATFORM_LINUX)
    list(APPEND PLATFORM_SOURCES
        src/Platform/Linux/LinuxWindow.cpp
        src/Platform/Linux/LinuxFileSystem.cpp
        src/Platform/Linux/LinuxApplication.cpp
    )
elseif(PLATFORM_ANDROID)
    list(APPEND PLATFORM_SOURCES
        src/Platform/Android/AndroidWindow.cpp
        src/Platform/Android/AndroidFileSystem.cpp
        src/Platform/Android/AndroidApplication.cpp
    )
endif()

# 核心库
add_library(EgretCore STATIC
    # 核心文件
    src/Core/Application.cpp
    src/Core/Display/DisplayObject.cpp
    src/Core/Display/DisplayObjectContainer.cpp
    src/Core/Display/Stage.cpp
    src/Core/Events/Event.cpp
    src/Core/Events/EventDispatcher.cpp
    
    # 平台特定文件
    ${PLATFORM_SOURCES}
)

target_link_libraries(EgretCore 
    OpenGL::GL 
    glfw
)

if(PLATFORM_WINDOWS)
    target_link_libraries(EgretCore GLEW::GLEW)
endif()
```

---

## 性能优化考虑

### 内存优化策略

#### 对象池模式
```cpp
// 通用对象池
template<typename T>
class ObjectPool {
public:
    template<typename... Args>
    std::shared_ptr<T> acquire(Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_pool.empty()) {
            return std::shared_ptr<T>(
                new T(std::forward<Args>(args)...),
                [this](T* obj) { this->release(obj); }
            );
        }
        
        auto obj = m_pool.back();
        m_pool.pop_back();
        
        // 重新初始化对象
        if constexpr (std::is_constructible_v<T, Args...>) {
            *obj = T(std::forward<Args>(args)...);
        }
        
        return std::shared_ptr<T>(obj, [this](T* obj) { this->release(obj); });
    }
    
    void release(T* obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_pool.size() < m_maxSize) {
            m_pool.push_back(obj);
        } else {
            delete obj;
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto* obj : m_pool) {
            delete obj;
        }
        m_pool.clear();
    }
    
private:
    std::vector<T*> m_pool;
    std::mutex m_mutex;
    size_t m_maxSize = 100;
};

// 事件对象池
class EventPool {
public:
    static std::shared_ptr<TouchEvent> createTouchEvent(
        const std::string& type, float x, float y, int touchId) {
        static ObjectPool<TouchEvent> pool;
        return pool.acquire(type, x, y, touchId);
    }
    
    static std::shared_ptr<Event> createEvent(
        const std::string& type, bool bubbles = false) {
        static ObjectPool<Event> pool;
        return pool.acquire(type, bubbles);
    }
};
```

#### 内存布局优化
```cpp
// SOA (Structure of Arrays) vs AOS (Array of Structures)
class ParticleSystem {
public:
    // SOA布局 - 更好的缓存局部性
    struct ParticleData {
        std::vector<Vector3> positions;
        std::vector<Vector3> velocities;
        std::vector<float> lifetimes;
        std::vector<Color> colors;
        std::vector<float> sizes;
        size_t count = 0;
        size_t capacity = 0;
        
        void resize(size_t newCapacity) {
            positions.resize(newCapacity);
            velocities.resize(newCapacity);
            lifetimes.resize(newCapacity);
            colors.resize(newCapacity);
            sizes.resize(newCapacity);
            capacity = newCapacity;
        }
    } m_particles;
    
    void update(float deltaTime) {
        // SIMD友好的更新循环
        for (size_t i = 0; i < m_particles.count; ++i) {
            m_particles.positions[i] += m_particles.velocities[i] * deltaTime;
            m_particles.lifetimes[i] -= deltaTime;
        }
        
        // 移除死亡的粒子
        removeDeadParticles();
    }
    
private:
    void removeDeadParticles() {
        size_t writeIndex = 0;
        for (size_t readIndex = 0; readIndex < m_particles.count; ++readIndex) {
            if (m_particles.lifetimes[readIndex] > 0.0f) {
                if (writeIndex != readIndex) {
                    m_particles.positions[writeIndex] = m_particles.positions[readIndex];
                    m_particles.velocities[writeIndex] = m_particles.velocities[readIndex];
                    m_particles.lifetimes[writeIndex] = m_particles.lifetimes[readIndex];
                    m_particles.colors[writeIndex] = m_particles.colors[readIndex];
                    m_particles.sizes[writeIndex] = m_particles.sizes[readIndex];
                }
                ++writeIndex;
            }
        }
        m_particles.count = writeIndex;
    }
};
```

### 渲染优化

#### 批处理和实例化渲染
```cpp
// 实例化渲染
class InstancedRenderer {
public:
    struct InstanceData {
        Matrix transform;
        Color color;
        Vector4 texCoordOffsetScale; // x,y=offset, z,w=scale
    };
    
    void addInstance(const InstanceData& instance) {
        m_instances.push_back(instance);
    }
    
    void render(RenderContext& context, std::shared_ptr<Mesh> mesh, 
                std::shared_ptr<Texture> texture) {
        if (m_instances.empty()) return;
        
        // 更新实例缓冲区
        updateInstanceBuffer();
        
        // 设置渲染状态
        context.setTexture(texture);
        context.setShader(m_instancedShader);
        
        // 实例化绘制
        context.drawInstanced(*mesh, m_instances.size());
        
        m_instances.clear();
    }
    
private:
    std::vector<InstanceData> m_instances;
    std::unique_ptr<Buffer> m_instanceBuffer;
    std::shared_ptr<Shader> m_instancedShader;
    
    void updateInstanceBuffer() {
        if (!m_instanceBuffer || m_instanceBuffer->size() < m_instances.size() * sizeof(InstanceData)) {
            m_instanceBuffer = std::make_unique<Buffer>(
                m_instances.size() * sizeof(InstanceData) * 2, // 预分配2倍空间
                Buffer::DYNAMIC_DRAW
            );
        }
        
        m_instanceBuffer->update(m_instances.data(), 
                                m_instances.size() * sizeof(InstanceData));
    }
};

// 动态批处理
class DynamicBatchRenderer {
public:
    void addQuad(const Vector3& position, const Vector2& size, 
                 const Rectangle& texCoords, const Color& color,
                 std::shared_ptr<Texture> texture) {
        // 检查是否可以批处理
        if (m_currentTexture != texture) {
            flush();
            m_currentTexture = texture;
        }
        
        // 生成四边形顶点
        generateQuad(position, size, texCoords, color);
        
        // 检查缓冲区是否已满
        if (m_vertexCount >= MAX_VERTICES - 4) {
            flush();
        }
    }
    
    void flush() {
        if (m_vertexCount == 0) return;
        
        // 更新顶点缓冲区
        m_vertexBuffer->update(m_vertices.data(), m_vertexCount * sizeof(Vertex));
        
        // 渲染
        m_renderContext->setTexture(m_currentTexture);
        m_renderContext->drawElements(*m_vertexBuffer, *m_indexBuffer, 
                                     m_vertexCount / 4 * 6);
        
        m_vertexCount = 0;
    }
    
private:
    static constexpr size_t MAX_VERTICES = 4096;
    
    std::vector<Vertex> m_vertices;
    std::unique_ptr<VertexBuffer> m_vertexBuffer;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<Texture> m_currentTexture;
    RenderContext* m_renderContext;
    size_t m_vertexCount = 0;
    
    void generateQuad(const Vector3& position, const Vector2& size,
                      const Rectangle& texCoords, const Color& color);
};
```

---

## 开发工具链

### 资源管道工具

#### 资源打包器
```cpp
// 资源打包工具
class AssetPacker {
public:
    struct PackageInfo {
        std::string name;
        std::vector<std::string> assets;
        CompressionType compression = CompressionType::ZLIB;
        bool encrypted = false;
    };
    
    bool packAssets(const std::string& configPath, const std::string& outputPath);
    
private:
    struct AssetEntry {
        std::string path;
        uint64_t offset;
        uint64_t size;
        uint64_t compressedSize;
        uint32_t crc32;
        AssetType type;
    };
    
    bool compressAsset(const std::vector<uint8_t>& input, 
                      std::vector<uint8_t>& output,
                      CompressionType type);
    void writePackageHeader(std::ofstream& file, const std::vector<AssetEntry>& entries);
};

// 使用示例配置
/*
assets.json:
{
    "packages": [
        {
            "name": "textures",
            "assets": [
                "images/*.png",
                "images/*.jpg"
            ],
            "compression": "zlib",
            "encrypted": false
        },
        {
            "name": "sounds", 
            "assets": [
                "audio/*.wav",
                "audio/*.ogg"
            ],
            "compression": "lz4",
            "encrypted": true
        }
    ]
}
*/
```

#### 纹理图集生成
```cpp
// 纹理图集打包
class TextureAtlasPacker {
public:
    struct AtlasInfo {
        struct Frame {
            std::string name;
            Rectangle rect;
            Rectangle sourceSize;
            Rectangle trimmed;
            bool rotated;
        };
        
        int width, height;
        std::vector<Frame> frames;
        std::string texturePath;
    };
    
    AtlasInfo packTextures(const std::vector<std::string>& imagePaths,
                          int maxWidth = 2048, int maxHeight = 2048,
                          int padding = 2, bool allowRotation = true);
    
    bool saveAtlas(const AtlasInfo& atlas, const std::string& outputPath);
    
private:
    // 矩形装箱算法
    class RectanglePacker {
    public:
        struct Node {
            Rectangle rect;
            std::unique_ptr<Node> left, right;
            bool used = false;
        };
        
        std::unique_ptr<Node> root;
        
        Rectangle insert(int width, int height);
        Node* insertNode(Node* node, int width, int height);
        Node* splitNode(Node* node, int width, int height);
    };
    
    RectanglePacker m_packer;
};
```

### 场景编辑器架构

#### 编辑器核心
```cpp
// 场景编辑器
class SceneEditor {
public:
    // 编辑器状态
    enum class Mode {
        SELECT,
        MOVE,
        ROTATE, 
        SCALE,
        CREATE
    };
    
    // 编辑器接口
    void setMode(Mode mode) { m_mode = mode; }
    void selectObject(std::shared_ptr<DisplayObject> object);
    void deleteSelected();
    void copySelected();
    void pasteClipboard();
    
    // 场景操作
    bool saveScene(const std::string& path);
    bool loadScene(const std::string& path);
    void newScene();
    
    // 历史记录
    void undo();
    void redo();
    
private:
    Mode m_mode = Mode::SELECT;
    std::shared_ptr<Stage> m_scene;
    std::vector<std::shared_ptr<DisplayObject>> m_selectedObjects;
    
    // 编辑器组件
    std::unique_ptr<PropertyPanel> m_propertyPanel;
    std::unique_ptr<SceneHierarchy> m_hierarchy;
    std::unique_ptr<AssetBrowser> m_assetBrowser;
    
    // 历史管理
    class CommandHistory {
    public:
        void execute(std::unique_ptr<Command> command);
        void undo();
        void redo();
        
    private:
        std::vector<std::unique_ptr<Command>> m_history;
        int m_currentIndex = -1;
    };
    
    CommandHistory m_commandHistory;
    
    // 编辑命令
    class MoveCommand : public Command {
    public:
        MoveCommand(std::shared_ptr<DisplayObject> obj, 
                   const Vector2& from, const Vector2& to)
            : m_object(obj), m_from(from), m_to(to) {}
        
        void execute() override {
            if (auto obj = m_object.lock()) {
                obj->setPosition(m_to);
            }
        }
        
        void undo() override {
            if (auto obj = m_object.lock()) {
                obj->setPosition(m_from);
            }
        }
        
    private:
        std::weak_ptr<DisplayObject> m_object;
        Vector2 m_from, m_to;
    };
};
```

### 构建系统和自动化

#### CMake配置完整版
```cmake
# 顶层CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(EgretEngine VERSION 5.4.1 LANGUAGES CXX)

# 编译选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 编译器特定选项
if(MSVC)
    add_compile_options(/W4 /WX- /bigobj)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -DDEBUG)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# 查找依赖
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)

# 可选依赖
find_package(Vulkan)
find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    pkg_check_modules(OPENAL REQUIRED openal)
endif()

# 第三方库子目录
add_subdirectory(third_party/stb)
add_subdirectory(third_party/nlohmann_json)
add_subdirectory(third_party/imgui)

# 引擎核心库
add_subdirectory(src/Core)
add_subdirectory(src/Rendering)
add_subdirectory(src/Resources)
add_subdirectory(src/Audio)
add_subdirectory(src/Platform)

# 工具
option(BUILD_TOOLS "Build development tools" ON)
if(BUILD_TOOLS)
    add_subdirectory(tools/AssetPacker)
    add_subdirectory(tools/TextureAtlas)
    add_subdirectory(tools/SceneEditor)
endif()

# 示例程序
option(BUILD_EXAMPLES "Build example programs" OFF)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# 测试
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装配置
include(GNUInstallDirs)
install(TARGETS EgretCore
    EXPORT EgretEngineTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 导出配置
install(EXPORT EgretEngineTargets
    FILE EgretEngineTargets.cmake
    NAMESPACE EgretEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EgretEngine
)
```

#### 自动化构建脚本
```bash
#!/bin/bash
# build.sh - 跨平台构建脚本

set -e

# 配置
BUILD_TYPE=${1:-Debug}
PLATFORM=${2:-$(uname)}
BUILD_DIR="build_${PLATFORM,,}_${BUILD_TYPE,,}"

echo "Building for $PLATFORM in $BUILD_TYPE mode..."

# 创建构建目录
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# 平台特定配置
case $PLATFORM in
    "Windows_NT"|"MINGW"*)
        cmake .. -G "Visual Studio 16 2019" \
            -A x64 \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DBUILD_TOOLS=ON \
            -DBUILD_EXAMPLES=ON
        cmake --build . --config $BUILD_TYPE
        ;;
    "Linux")
        cmake .. -G "Unix Makefiles" \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DBUILD_TOOLS=ON \
            -DBUILD_EXAMPLES=ON
        make -j$(nproc)
        ;;
    "Darwin")
        cmake .. -G "Xcode" \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DBUILD_TOOLS=ON \
            -DBUILD_EXAMPLES=ON
        xcodebuild -configuration $BUILD_TYPE
        ;;
    *)
        echo "Unsupported platform: $PLATFORM"
        exit 1
        ;;
esac

echo "Build completed successfully!"
```

---

## 实施路线图

### 第一阶段：基础框架 (1-2个月)

#### Week 1-2: 项目搭建
- [x] **开发环境搭建**
  - CMake构建系统配置
  - 第三方依赖集成 (OpenGL, GLFW, stb_image等)
  - 跨平台编译测试

- [x] **核心基础类**
  ```cpp
  // 优先实现的类
  - Object (基础对象类)
  - Rectangle, Point, Matrix (几何类)
  - Color (颜色类)
  - Vector2, Vector3, Vector4 (向量类)
  ```

#### Week 3-4: 事件系统
- [x] **事件基础设施**
  ```cpp
  - Event (事件基类)
  - EventDispatcher (事件分发器)
  - TouchEvent, MouseEvent (输入事件)
  ```

- [x] **简单测试程序**
  - 窗口创建和事件处理
  - 基础几何变换验证

#### Week 5-6: 显示系统核心
- [x] **显示对象层次**
  ```cpp
  - DisplayObject (基础显示对象)
  - DisplayObjectContainer (容器)
  - Stage (舞台)
  ```

#### Week 7-8: 基础渲染
- [x] **OpenGL渲染后端**
  ```cpp
  - RenderContext (渲染上下文)
  - Shader, Texture, Buffer (渲染资源)
  - 简单的三角形和四边形渲染
  ```

### 第二阶段：核心功能 (2-3个月)

#### Week 9-12: 资源系统
- [x] **资源管理**
  ```cpp
  - Resource (资源基类)
  - ResourceManager (资源管理器)
  - ImageLoader (图片加载器)
  - Texture (纹理类)
  ```

- [x] **文件系统抽象**
  - 跨平台文件访问
  - 资源包格式定义
  - 异步加载机制

#### Week 13-16: 完整显示系统
- [x] **显示对象扩展**
  ```cpp
  - Bitmap (位图显示)
  - Shape (矢量图形)
  - TextField (文本显示)
  - MovieClip (动画剪辑)
  ```

#### Week 17-20: 输入和交互
- [x] **输入处理**
  - 多平台输入适配
  - 触摸和鼠标统一处理
  - 键盘输入支持

- [x] **碰撞检测**
  - 点击测试算法
  - 边界框碰撞
  - 像素级碰撞（可选）

### 第三阶段：高级功能 (2-3个月)

#### Week 21-24: 动画系统
- [x] **Tween动画**
  ```cpp
  - Tween (补间动画类)
  - Ease (缓动函数)
  - TweenGroup (动画组)
  ```

#### Week 25-28: 音频系统
- [x] **音频支持**
  ```cpp
  - Sound (音频类)
  - SoundChannel (音频通道)
  - AudioContext (音频上下文)
  ```

#### Week 29-32: 网络功能
- [x] **网络通信**
  ```cpp
  - HttpRequest (HTTP请求)
  - WebSocket (WebSocket连接)
  - JSON解析和序列化
  ```

### 第四阶段：优化和工具 (2-3个月)

#### Week 33-36: 性能优化
- [x] **渲染优化**
  - 批处理系统
  - 遮挡剔除
  - 纹理图集支持

- [x] **内存优化**
  - 对象池实现
  - 智能指针优化
  - 缓存友好的数据结构

#### Week 37-40: 开发工具
- [x] **资源工具**
  - 纹理打包器
  - 资源压缩工具
  - 资源包生成器

#### Week 41-44: 跨平台适配
- [x] **平台支持**
  - Windows完整支持
  - Linux完整支持
  - macOS基础支持
  - Android/iOS预研

### 第五阶段：扩展和完善 (2个月)

#### Week 45-48: UI系统
- [x] **基础UI组件**
  ```cpp
  - Button (按钮)
  - Label (标签)
  - Panel (面板)
  - ScrollView (滚动视图)
  ```

#### Week 49-52: 最终完善
- [x] **文档和示例**
  - API文档生成
  - 教程和示例程序
  - 性能测试用例

- [x] **质量保证**
  - 单元测试覆盖
  - 内存泄漏检测
  - 性能基准测试

### 关键里程碑

#### 里程碑1: 基础演示 (Week 8)
- **目标**: 能够创建窗口，显示简单图形，响应鼠标事件
- **演示内容**: 
  - 显示多个可交互的彩色矩形
  - 鼠标点击移动矩形
  - 简单的变换动画

#### 里程碑2: 图片显示 (Week 16) 
- **目标**: 完整的图片加载和显示功能
- **演示内容**:
  - 加载和显示PNG/JPG图片
  - 图片的缩放、旋转、透明度变化
  - 简单的精灵动画

#### 里程碑3: 游戏原型 (Week 24)
- **目标**: 能够制作简单的2D游戏
- **演示内容**:
  - 简单的打飞机游戏
  - 包含音效和背景音乐
  - 碰撞检测和得分系统

#### 里程碑4: 完整引擎 (Week 40)
- **目标**: 功能完整的游戏引擎
- **演示内容**:
  - 复杂的2D游戏演示
  - 完整的UI系统
  - 资源热加载功能

#### 里程碑5: 产品级引擎 (Week 52)
- **目标**: 可用于商业项目的引擎
- **交付物**:
  - 引擎运行库
  - 开发工具套件
  - 完整文档和教程

---

## 技术难点和解决方案

### 主要挑战

#### 1. 内存管理复杂性
**挑战**: JavaScript的GC vs C++的手动内存管理

**解决方案**:
```cpp
// 引用计数 + 弱引用避免循环引用
class DisplayObject : public std::enable_shared_from_this<DisplayObject> {
    std::vector<std::shared_ptr<DisplayObject>> children;
    std::weak_ptr<DisplayObjectContainer> parent; // 避免循环引用
};

// RAII + 智能指针
class ResourceHandle {
    std::unique_ptr<Resource> resource;
    ~ResourceHandle() { /* 自动清理 */ }
};
```

#### 2. 异步操作处理
**挑战**: Promise/async-await vs C++的异步模式

**解决方案**:
```cpp
// 基于std::future的异步系统
template<typename T>
class AsyncResult {
public:
    std::future<T> future;
    
    template<typename Func>
    auto then(Func&& callback) -> AsyncResult<decltype(callback(T{}))> {
        return AsyncResult<decltype(callback(T{}))>{
            std::async(std::launch::async, [f = std::move(future), 
                                          cb = std::forward<Func>(callback)]() mutable {
                return cb(f.get());
            })
        };
    }
};

// 协程支持 (C++20)
#if __cpp_impl_coroutine
#include <coroutine>

task<std::shared_ptr<Texture>> loadTextureAsync(const std::string& path) {
    auto data = co_await loadFileAsync(path);
    auto texture = co_await createTextureAsync(data);
    co_return texture;
}
#endif
```

#### 3. 跨平台兼容性
**挑战**: 不同平台的API差异

**解决方案**:
```cpp
// 平台抽象层 + 编译时多态
#ifdef PLATFORM_WINDOWS
    #include "WindowsSpecific.h"
    using PlatformImplementation = WindowsImplementation;
#elif defined(PLATFORM_LINUX)
    #include "LinuxSpecific.h" 
    using PlatformImplementation = LinuxImplementation;
#endif

// 统一接口
class PlatformAPI : public PlatformImplementation {
    // 继承平台特定实现
};
```

### 性能关键点

#### 渲染性能优化
```cpp
// 1. 状态变化最小化
class RenderStateCache {
    GLuint currentProgram = 0;
    GLuint currentTexture = 0;
    
    void useProgram(GLuint program) {
        if (program != currentProgram) {
            glUseProgram(program);
            currentProgram = program;
        }
    }
};

// 2. 几何数据预计算
class DisplayObject {
    mutable Matrix m_worldTransform;
    mutable bool m_transformDirty = true;
    
    const Matrix& getWorldTransform() const {
        if (m_transformDirty) {
            updateWorldTransform();
            m_transformDirty = false;
        }
        return m_worldTransform;
    }
};

// 3. 空间分割优化
class QuadTree {
    // 用于减少不必要的渲染和碰撞检测
    std::vector<std::weak_ptr<DisplayObject>> objects;
    std::array<std::unique_ptr<QuadTree>, 4> children;
    
    void queryRange(const Rectangle& range, 
                   std::vector<std::shared_ptr<DisplayObject>>& result);
};
```

---

## 测试策略

### 单元测试框架
```cpp
// 使用Google Test
#include <gtest/gtest.h>

class DisplayObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        stage = std::make_shared<Stage>();
        container = std::make_shared<DisplayObjectContainer>();
        child = std::make_shared<TestDisplayObject>();
    }
    
    std::shared_ptr<Stage> stage;
    std::shared_ptr<DisplayObjectContainer> container;
    std::shared_ptr<TestDisplayObject> child;
};

TEST_F(DisplayObjectTest, AddRemoveChild) {
    EXPECT_EQ(container->numChildren(), 0);
    
    container->addChild(child);
    EXPECT_EQ(container->numChildren(), 1);
    EXPECT_EQ(child->parent, container.get());
    
    container->removeChild(child);
    EXPECT_EQ(container->numChildren(), 0);
    EXPECT_EQ(child->parent, nullptr);
}

TEST_F(DisplayObjectTest, Transform) {
    child->x = 100;
    child->y = 50;
    child->scaleX = 2.0f;
    
    Matrix transform = child->getTransform();
    Point result = transform.transformPoint(Point(0, 0));
    
    EXPECT_FLOAT_EQ(result.x, 100);
    EXPECT_FLOAT_EQ(result.y, 50);
}
```

### 集成测试
```cpp
// 渲染测试
class RenderingTest : public ::testing::Test {
protected:
    void SetUp() override {
        window = std::make_unique<TestWindow>(800, 600);
        context = std::make_unique<OpenGLRenderContext>(window.get());
        stage = std::make_shared<Stage>();
    }
    
    std::unique_ptr<TestWindow> window;
    std::unique_ptr<RenderContext> context;
    std::shared_ptr<Stage> stage;
};

TEST_F(RenderingTest, BasicRendering) {
    // 创建测试场景
    auto bitmap = std::make_shared<Bitmap>(loadTestTexture("test.png"));
    bitmap->x = 100;
    bitmap->y = 100;
    stage->addChild(bitmap);
    
    // 渲染
    context->clear(Color::BLACK);
    stage->render(*context);
    context->present();
    
    // 验证渲染结果 (像素对比或视觉回归测试)
    auto screenshot = context->captureScreen();
    EXPECT_TRUE(compareWithReference(screenshot, "expected_output.png"));
}
```

### 性能测试
```cpp
// 性能基准测试
class PerformanceBenchmark : public ::testing::Test {
protected:
    static constexpr int NUM_OBJECTS = 10000;
    
    void SetUp() override {
        stage = std::make_shared<Stage>();
        
        // 创建大量对象
        for (int i = 0; i < NUM_OBJECTS; ++i) {
            auto obj = std::make_shared<TestDisplayObject>();
            obj->x = i % 100;
            obj->y = i / 100;
            stage->addChild(obj);
        }
    }
    
    std::shared_ptr<Stage> stage;
};

TEST_F(PerformanceBenchmark, RenderingPerformance) {
    auto context = createTestRenderContext();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int frame = 0; frame < 1000; ++frame) {
        // 更新所有对象
        stage->update(1.0f/60.0f);
        
        // 渲染
        context->clear(Color::BLACK);
        stage->render(*context);
        context->present();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double fps = 1000.0 * 1000 / duration.count();
    EXPECT_GT(fps, 30.0); // 至少30FPS
    
    std::cout << "Rendering FPS: " << fps << std::endl;
}
```

---

## 总结和建议

### 项目成功关键因素

#### 1. **渐进式开发策略**
- 先实现MVP (最小可行产品)
- 每个阶段都要有可演示的成果
- 及早发现和解决架构问题

#### 2. **质量优先原则**
- 代码规范和一致性
- 全面的测试覆盖
- 持续的性能监控

#### 3. **文档和社区**
- 详细的API文档
- 丰富的示例和教程
- 活跃的开发者社区

### 技术选择建议

#### 编程范式
- **现代C++**: 使用C++17/20特性
- **RAII**: 资源管理的黄金法则
- **智能指针**: 内存安全的保障

#### 第三方库选择
- **必选**: OpenGL, GLFW, stb_image
- **推荐**: nlohmann/json, spdlog, fmt
- **可选**: Vulkan, DirectX12 (高级渲染)

#### 构建系统
- **CMake**: 跨平台构建的标准选择
- **Conan/vcpkg**: 依赖管理工具
- **CI/CD**: GitHub Actions或Jenkins

### 风险评估和缓解

#### 技术风险
- **复杂度爆炸**: 通过模块化和分层设计控制
- **性能问题**: 早期建立性能基准和测试
- **兼容性问题**: 分阶段支持不同平台

#### 项目风险  
- **时间估算偏差**: 预留20-30%的缓冲时间
- **人力资源不足**: 优先核心功能，延后可选特性
- **需求变更**: 保持架构的灵活性和扩展性

### 最终建议

这是一个极具挑战性但非常有价值的项目。C++版白鹭引擎不仅能提供更好的性能，还能为游戏开发者提供更多的控制能力和部署选择。

**建议的启动步骤**:

1. **搭建基础框架** (第1个月)
2. **实现核心显示系统** (第2-3个月)  
3. **完善资源管理** (第4个月)
4. **添加动画和音频** (第5-6个月)
5. **优化和跨平台适配** (第7-8个月)
6. **开发工具和文档** (第9-12个月)

成功完成这个项目将创造一个强大的2D游戏引擎，为中国的游戏开发生态做出重要贡献！

---

*文档完成时间: 2025年1月*  
*目标引擎版本: Egret Engine 5.4.1 C++ Port*  
*预计开发周期: 12个月*  
*文档状态: 完整技术指南*