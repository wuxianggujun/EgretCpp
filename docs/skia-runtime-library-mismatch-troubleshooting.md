# Skia 运行时库不匹配问题排查指南

## 问题描述

在集成 Skia 图形库时遇到链接错误，提示运行时库不匹配：

```
skia.lib(skia.SkMemory_malloc.obj) : error LNK2038: 检测到"_ITERATOR_DEBUG_LEVEL"的不匹配项: 值"0"不匹配值"2"(main.cpp.obj 中)
skia.lib(skia.SkMemory_malloc.obj) : error LNK2038: 检测到"RuntimeLibrary"的不匹配项: 值"MT_StaticRelease"不匹配值"MDd_DynamicDebug"(main.cpp.obj 中)
```

## 错误分析

### 错误信息解读
- `_ITERATOR_DEBUG_LEVEL` 不匹配：项目使用值"2"（Debug模式），Skia库使用值"0"（Release模式）
- `RuntimeLibrary` 不匹配：项目使用 `MDd_DynamicDebug`（动态链接Debug库），Skia库使用 `MT_StaticRelease`（静态链接Release库）

### 根本原因
即使下载的是标记为"Debug"的 Skia 库，实际编译时仍可能使用了 Release 设置，导致：
1. 运行时库类型不匹配（静态 vs 动态）
2. 调试级别设置不匹配
3. 项目编译设置与库编译设置不一致

## 解决方案

### 方案一：修改项目设置匹配 Skia 库（临时解决）

**CMakeLists.txt 修改：**
```cmake
# 匹配 Skia 的实际运行时库设置（静态Release）
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
    # 匹配 Skia 的迭代器调试级别
    add_compile_definitions(NDEBUG _ITERATOR_DEBUG_LEVEL=0)
endif()
```

**FindSkia.cmake 修改：**
```cmake
# 设置编译定义以匹配实际 Skia 库编译
target_compile_definitions(Skia::Skia INTERFACE
    SK_RELEASE
    NDEBUG
    _ITERATOR_DEBUG_LEVEL=0
    SK_DISABLE_LEGACY_SHADERCONTEXT
)
```

### 方案二：正确编译 Skia 库（推荐解决）

确保 Skia 库使用正确的构建配置：

**Debug 配置：**
- 运行时库：`/MDd` (MultiThreadedDebugDLL)
- 迭代器调试级别：`_ITERATOR_DEBUG_LEVEL=2`
- 编译定义：`_DEBUG`, `SK_DEBUG`

**Release 配置：**
- 运行时库：`/MD` (MultiThreadedDLL) 或 `/MT` (MultiThreaded)
- 迭代器调试级别：`_ITERATOR_DEBUG_LEVEL=0`
- 编译定义：`NDEBUG`, `SK_RELEASE`

## 排查步骤

### 1. 检查库文件编译设置
```bash
# 检查库文件大小（Debug版本通常更大）
ls -la third_party/Skia/out/Debug-windows-x64/skia.lib
ls -la third_party/Skia/out/Release-windows-x64/skia.lib
```

### 2. 清理 CMake 缓存
```bash
# 删除构建目录强制重新配置
rm -rf cmake-build-*
```

### 3. 在 FindSkia.cmake 中添加调试信息
```cmake
# 添加调试输出
message(STATUS "Using Skia library: ${SKIA_MAIN_LIB}")
message(STATUS "Skia lib dir: ${SKIA_LIB_DIR}")
```

### 4. 清理 CMake 库缓存
```cmake
# 强制重新检测库文件
unset(SKIA_MAIN_LIB CACHE)
unset(HARFBUZZ_LIB CACHE)
unset(ICU_LIB CACHE)
# ... 其他库变量
```

## 最佳实践

### 1. 运行时库一致性
- 确保项目和所有依赖库使用相同的运行时库类型
- Debug 构建使用 Debug 运行时库，Release 构建使用 Release 运行时库

### 2. CMake 配置建议
```cmake
# 设置 MSVC 运行时库策略
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

# 基于构建类型配置运行时库
if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()
endif()
```

### 3. Skia 集成配置
```cmake
# 动态选择库目录
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SKIA_LIB_DIR ${SKIA_ROOT}/out/Debug-windows-x64)
else()
    set(SKIA_LIB_DIR ${SKIA_ROOT}/out/Release-windows-x64)
endif()

# 匹配编译定义
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Skia::Skia INTERFACE
        SK_DEBUG
        _DEBUG
        _ITERATOR_DEBUG_LEVEL=2
    )
else()
    target_compile_definitions(Skia::Skia INTERFACE
        SK_RELEASE
        NDEBUG
        _ITERATOR_DEBUG_LEVEL=0
        SK_DISABLE_LEGACY_SHADERCONTEXT
    )
endif()
```

## 常见陷阱

1. **库标签误导**：标记为"Debug"的库可能实际上是 Release 编译
2. **CMake 缓存**：修改配置后需要清理缓存才能生效
3. **混合运行时库**：不同的运行时库类型无法混合使用
4. **调试级别不匹配**：`_ITERATOR_DEBUG_LEVEL` 必须在所有模块中保持一致

## 验证方法

成功解决后，程序应该能够正常编译和运行：
```
Hello, World with Skia!
Successfully created skia_test.png!

进程已结束，退出代码为 0
```

## 相关文件

- `cmake/FindSkia.cmake` - Skia 库配置
- `CMakeLists.txt` - 主项目配置
- `main.cpp` - 示例应用程序

## 更新记录

- 2025-09-03: 初始文档创建，记录运行时库不匹配问题及解决方案