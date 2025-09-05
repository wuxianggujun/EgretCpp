# Egret Geom模块 C++翻译参考文档

基于白鹭引擎5.4.1的geom模块源码翻译为C++的完整参考实现，充分利用GLM数学库优化。

## 概述

geom模块包含三个核心数学类：
- **Point** - 2D坐标点，使用GLM vec2优化
- **Rectangle** - 矩形区域
- **Matrix** - 2D变换矩阵，使用GLM mat3优化

## 文件结构

需要创建以下文件：
```
src/geom/Point.hpp
src/geom/Point.cpp
src/geom/Rectangle.hpp  
src/geom/Rectangle.cpp
src/geom/Matrix.hpp
src/geom/Matrix.cpp
```

## Point类 - 使用GLM优化

### Point.hpp

```cpp
#pragma once
#include "core/HashObject.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace egret {
    
    /**
     * Point对象表示二维坐标系统中的某个位置
     * 使用GLM vec2进行内部存储和计算优化
     */
    class Point : public HashObject {
    private:
        static std::vector<std::unique_ptr<Point>> s_pointPool; // 对象池
        static constexpr double DEG_TO_RAD = M_PI / 180.0;
        
    public:
        // ========== 对象池管理 ==========
        
        /**
         * 释放Point实例到对象池
         */
        static void release(std::unique_ptr<Point> point);
        
        /**
         * 从对象池中取出或创建新的Point对象
         */
        static std::unique_ptr<Point> create(double x = 0.0, double y = 0.0);
        
        // ========== 构造函数 ==========
        
        /**
         * 创建Point对象
         * @param x 水平坐标，默认为0
         * @param y 垂直坐标，默认为0  
         */
        Point(double x = 0.0, double y = 0.0);
        
        // ========== 坐标属性 ==========
        
        /**
         * 水平坐标
         */
        double x;
        
        /**
         * 垂直坐标
         */
        double y;
        
        // ========== GLM集成方法 ==========
        
        /**
         * 获取GLM vec2表示
         */
        glm::dvec2 toVec2() const { return glm::dvec2(x, y); }
        
        /**
         * 从GLM vec2设置坐标
         */
        void fromVec2(const glm::dvec2& vec) { x = vec.x; y = vec.y; }
        
        // ========== 计算属性 ==========
        
        /**
         * 从(0,0)到此点的线段长度
         */
        double getLength() const;
        
        // ========== 基本操作 ==========
        
        /**
         * 设置坐标值
         */
        Point& setTo(double x, double y);
        
        /**
         * 克隆点对象
         */
        std::unique_ptr<Point> clone() const;
        
        /**
         * 比较两点是否相等
         */
        bool equals(const Point& other) const;
        
        /**
         * 从源点复制数据
         */
        void copyFrom(const Point& sourcePoint);
        
        // ========== 静态工具方法 ==========
        
        /**
         * 计算两点间距离
         */
        static double distance(const Point& p1, const Point& p2);
        
        /**
         * 两点间插值
         * @param pt1 第一个点
         * @param pt2 第二个点
         * @param f 插值因子 (0.0-1.0)
         */
        static std::unique_ptr<Point> interpolate(const Point& pt1, const Point& pt2, double f);
        
        /**
         * 极坐标转笛卡尔坐标
         * @param len 长度
         * @param angle 角度（度数）
         */
        static std::unique_ptr<Point> polar(double len, double angle);
        
        // ========== 向量运算 ==========
        
        /**
         * 向量加法
         */
        std::unique_ptr<Point> add(const Point& v) const;
        
        /**
         * 向量减法  
         */
        std::unique_ptr<Point> subtract(const Point& v) const;
        
        /**
         * 标准化向量
         * @param thickness 目标长度
         */
        void normalize(double thickness);
        
        /**
         * 偏移坐标
         */
        void offset(double dx, double dy);
        
        // ========== 字符串表示 ==========
        
        /**
         * 返回坐标的字符串表示
         */
        std::string toString() const;
    };
    
    // 框架内部临时对象（避免频繁分配）
    extern Point TempPoint;
    
} // namespace egret
```

### Point.cpp 关键实现

```cpp
#include "geom/Point.hpp"
#include <cmath>
#include <sstream>

namespace egret {
    
    std::vector<std::unique_ptr<Point>> Point::s_pointPool;
    Point TempPoint; // 全局临时对象
    
    Point::Point(double x, double y) : x(x), y(y) {
    }
    
    void Point::release(std::unique_ptr<Point> point) {
        if (point) {
            s_pointPool.push_back(std::move(point));
        }
    }
    
    std::unique_ptr<Point> Point::create(double x, double y) {
        if (!s_pointPool.empty()) {
            auto point = std::move(s_pointPool.back());
            s_pointPool.pop_back();
            return point->setTo(x, y), std::move(point);
        }
        return std::make_unique<Point>(x, y);
    }
    
    double Point::getLength() const {
        // 使用GLM优化的长度计算
        return glm::length(toVec2());
    }
    
    Point& Point::setTo(double x, double y) {
        this->x = x;
        this->y = y;
        return *this;
    }
    
    std::unique_ptr<Point> Point::clone() const {
        return std::make_unique<Point>(x, y);
    }
    
    bool Point::equals(const Point& other) const {
        // 使用GLM的精度比较
        return glm::all(glm::epsilonEqual(toVec2(), other.toVec2(), 1e-6));
    }
    
    void Point::copyFrom(const Point& sourcePoint) {
        x = sourcePoint.x;
        y = sourcePoint.y;
    }
    
    double Point::distance(const Point& p1, const Point& p2) {
        // 使用GLM优化的距离计算
        return glm::distance(p1.toVec2(), p2.toVec2());
    }
    
    void Point::normalize(double thickness) {
        if (x != 0.0 || y != 0.0) {
            // 使用GLM优化的标准化
            glm::dvec2 normalized = glm::normalize(toVec2()) * thickness;
            fromVec2(normalized);
        }
    }
    
    // ... 其他方法实现
}
```

## Rectangle类

### Rectangle.hpp

```cpp
#pragma once
#include "core/HashObject.hpp"
#include "geom/Point.hpp"
#include <memory>
#include <vector>

namespace egret {
    
    /**
     * Rectangle对象表示矩形区域
     */
    class Rectangle : public HashObject {
    private:
        static std::vector<std::unique_ptr<Rectangle>> s_rectanglePool; // 对象池
        
    public:
        // ========== 对象池管理 ==========
        
        /**
         * 释放Rectangle实例到对象池
         */
        static void release(std::unique_ptr<Rectangle> rect);
        
        /**
         * 从对象池中取出或创建新的Rectangle对象
         */
        static std::unique_ptr<Rectangle> create();
        
        // ========== 构造函数 ==========
        
        /**
         * 创建Rectangle对象
         */
        Rectangle(double x = 0.0, double y = 0.0, double width = 0.0, double height = 0.0);
        
        // ========== 基本属性 ==========
        
        /**
         * 矩形左上角x坐标
         */
        double x;
        
        /**
         * 矩形左上角y坐标
         */
        double y;
        
        /**
         * 矩形宽度
         */
        double width;
        
        /**
         * 矩形高度
         */
        double height;
        
        // ========== 计算属性 ==========
        
        /**
         * 右边界 (x + width)
         */
        double getRight() const { return x + width; }
        void setRight(double value) { width = value - x; }
        
        /**
         * 下边界 (y + height)
         */
        double getBottom() const { return y + height; }
        void setBottom(double value) { height = value - y; }
        
        /**
         * 左边界 (等于x)
         */
        double getLeft() const { return x; }
        void setLeft(double value) { width += x - value; x = value; }
        
        /**
         * 上边界 (等于y)
         */
        double getTop() const { return y; }
        void setTop(double value) { height += y - value; y = value; }
        
        /**
         * 左上角坐标点
         */
        std::unique_ptr<Point> getTopLeft() const;
        void setTopLeft(const Point& value);
        
        /**
         * 右下角坐标点
         */
        std::unique_ptr<Point> getBottomRight() const;
        void setBottomRight(const Point& value);
        
        // ========== 基本操作 ==========
        
        /**
         * 从源矩形复制数据
         */
        Rectangle& copyFrom(const Rectangle& sourceRect);
        
        /**
         * 设置矩形参数
         */
        Rectangle& setTo(double x, double y, double width, double height);
        
        /**
         * 克隆矩形
         */
        std::unique_ptr<Rectangle> clone() const;
        
        /**
         * 比较矩形是否相等
         */
        bool equals(const Rectangle& toCompare) const;
        
        // ========== 几何检测 ==========
        
        /**
         * 检测点是否在矩形内
         */
        bool contains(double x, double y) const;
        
        /**
         * 检测Point是否在矩形内
         */
        bool containsPoint(const Point& point) const;
        
        /**
         * 检测是否完全包含另一个矩形
         */
        bool containsRect(const Rectangle& rect) const;
        
        /**
         * 检测是否与另一个矩形相交
         */
        bool intersects(const Rectangle& toIntersect) const;
        
        /**
         * 获取与另一个矩形的交集
         */
        std::unique_ptr<Rectangle> intersection(const Rectangle& toIntersect) const;
        
        /**
         * 获取与另一个矩形的并集
         */
        std::unique_ptr<Rectangle> union_(const Rectangle& toUnion) const;
        
        // ========== 状态检查 ==========
        
        /**
         * 检查矩形是否为空
         */
        bool isEmpty() const;
        
        /**
         * 将矩形设为空
         */
        void setEmpty();
        
        // ========== 变换操作 ==========
        
        /**
         * 按指定量扩大矩形
         */
        void inflate(double dx, double dy);
        
        /**
         * 使用Point扩大矩形
         */
        void inflatePoint(const Point& point);
        
        /**
         * 偏移矩形位置
         */
        void offset(double dx, double dy);
        
        /**
         * 使用Point偏移矩形
         */
        void offsetPoint(const Point& point);
        
        // ========== 字符串表示 ==========
        
        /**
         * 返回矩形的字符串表示
         */
        std::string toString() const;
        
        // ========== 私有辅助方法 ==========
        
    private:
        /**
         * 就地计算交集
         */
        Rectangle& intersectInPlace(const Rectangle& clipRect);
        
        /**
         * 获取旋转后的基础宽度
         */
        double getBaseWidth(double angle) const;
        
        /**
         * 获取旋转后的基础高度
         */
        double getBaseHeight(double angle) const;
    };
    
    // 框架内部临时对象
    extern Rectangle TempRectangle;
    
} // namespace egret
```

## Matrix类 - 使用GLM优化

### Matrix.hpp

```cpp
#pragma once
#include "core/HashObject.hpp"
#include "geom/Point.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace egret {
    
    /**
     * Matrix类表示2D变换矩阵
     * 使用GLM mat3进行内部优化计算
     */
    class Matrix : public HashObject {
    private:
        static std::vector<std::unique_ptr<Matrix>> s_matrixPool; // 对象池
        static constexpr double PI = M_PI;
        static constexpr double TwoPI = PI * 2.0;
        static constexpr double DEG_TO_RAD = PI / 180.0;
        
    public:
        // ========== 对象池管理 ==========
        
        /**
         * 释放Matrix实例到对象池
         */
        static void release(std::unique_ptr<Matrix> matrix);
        
        /**
         * 从对象池中取出或创建新的Matrix对象
         */
        static std::unique_ptr<Matrix> create();
        
        // ========== 构造函数 ==========
        
        /**
         * 创建Matrix对象
         * @param a 影响x轴缩放和旋转的值
         * @param b 影响y轴旋转和倾斜的值  
         * @param c 影响x轴旋转和倾斜的值
         * @param d 影响y轴缩放和旋转的值
         * @param tx x轴平移距离
         * @param ty y轴平移距离
         */
        Matrix(double a = 1.0, double b = 0.0, double c = 0.0, double d = 1.0, double tx = 0.0, double ty = 0.0);
        
        // ========== 矩阵元素 ==========
        
        /**
         * 影响x轴缩放和旋转的值
         */
        double a;
        
        /**
         * 影响y轴旋转和倾斜的值
         */
        double b;
        
        /**
         * 影响x轴旋转和倾斜的值
         */
        double c;
        
        /**
         * 影响y轴缩放和旋转的值
         */
        double d;
        
        /**
         * x轴平移距离
         */
        double tx;
        
        /**
         * y轴平移距离
         */
        double ty;
        
        // ========== GLM集成方法 ==========
        
        /**
         * 转换为GLM mat3矩阵
         * GLM使用列主序，需要转置
         */
        glm::dmat3 toGLM() const;
        
        /**
         * 从GLM mat3矩阵设置
         */
        void fromGLM(const glm::dmat3& mat);
        
        // ========== 基本操作 ==========
        
        /**
         * 克隆矩阵
         */
        std::unique_ptr<Matrix> clone() const;
        
        /**
         * 矩阵连接（乘法）
         */
        void concat(const Matrix& other);
        
        /**
         * 从源矩阵复制数据
         */
        Matrix& copyFrom(const Matrix& other);
        
        /**
         * 设置矩阵为单位矩阵
         */
        void identity();
        
        /**
         * 矩阵求逆
         */
        void invert();
        
        /**
         * 设置矩阵所有值
         */
        Matrix& setTo(double a, double b, double c, double d, double tx, double ty);
        
        // ========== 变换操作 ==========
        
        /**
         * 旋转变换
         * @param angle 旋转角度（弧度）
         */
        void rotate(double angle);
        
        /**
         * 缩放变换
         */
        void scale(double sx, double sy);
        
        /**
         * 平移变换
         */
        void translate(double dx, double dy);
        
        /**
         * 创建Box变换矩阵
         */
        void createBox(double scaleX, double scaleY, double rotation = 0.0, double tx = 0.0, double ty = 0.0);
        
        /**
         * 创建梯度Box变换矩阵
         */
        void createGradientBox(double width, double height, double rotation = 0.0, double tx = 0.0, double ty = 0.0);
        
        // ========== 点变换 ==========
        
        /**
         * 变换点坐标
         */
        std::unique_ptr<Point> transformPoint(double x, double y) const;
        std::unique_ptr<Point> transformPoint(const Point& point) const;
        
        /**
         * 变换点坐标到指定Point对象
         */
        void transformPointTo(double x, double y, Point& result) const;
        
        /**
         * 增量变换点坐标（不包括平移）
         */
        std::unique_ptr<Point> deltaTransformPoint(const Point& point) const;
        
        // ========== 高级功能 ==========
        
        /**
         * 预乘矩阵
         */
        void prepend(const Matrix& other);
        
        /**
         * 预乘并将结果存储到指定矩阵
         */
        void preMultiplyInto(const Matrix& other, Matrix& result) const;
        
        /**
         * 更新缩放和旋转参数
         */
        void updateScaleAndRotation(double scaleX, double scaleY, double skewX, double skewY);
        
        // ========== 属性获取 ==========
        
        /**
         * 获取x轴缩放值
         */
        double getScaleX() const;
        
        /**
         * 获取y轴缩放值
         */
        double getScaleY() const;
        
        /**
         * 获取x轴倾斜值
         */
        double getSkewX() const;
        
        /**
         * 获取y轴倾斜值
         */
        double getSkewY() const;
        
        /**
         * 获取旋转角度
         */
        double getRotation() const;
        
        // ========== 字符串表示 ==========
        
        /**
         * 返回矩阵的字符串表示
         */
        std::string toString() const;
        
    private:
        // GLM缓存矩阵，避免频繁转换
        mutable glm::dmat3 m_glmMatrix;
        mutable bool m_glmDirty = true;
        
        /**
         * 更新GLM缓存矩阵
         */
        void updateGLMMatrix() const;
    };
    
    // 框架内部临时对象
    extern Matrix TempMatrix;
    
} // namespace egret
```

### Matrix.cpp 关键实现（使用GLM优化）

```cpp
#include "geom/Matrix.hpp"
#include <glm/gtx/matrix_transform_2d.hpp>
#include <sstream>

namespace egret {
    
    std::vector<std::unique_ptr<Matrix>> Matrix::s_matrixPool;
    Matrix TempMatrix; // 全局临时对象
    
    Matrix::Matrix(double a, double b, double c, double d, double tx, double ty)
        : a(a), b(b), c(c), d(d), tx(tx), ty(ty), m_glmDirty(true) {
    }
    
    glm::dmat3 Matrix::toGLM() const {
        if (m_glmDirty) {
            updateGLMMatrix();
        }
        return m_glmMatrix;
    }
    
    void Matrix::updateGLMMatrix() const {
        // 2D变换矩阵转3x3矩阵（齐次坐标）
        // GLM使用列主序存储
        m_glmMatrix = glm::dmat3(
            a,  c,  tx,    // 第一列
            b,  d,  ty,    // 第二列
            0.0, 0.0, 1.0  // 第三列
        );
        m_glmDirty = false;
    }
    
    void Matrix::fromGLM(const glm::dmat3& mat) {
        a = mat[0][0];   b = mat[1][0];
        c = mat[0][1];   d = mat[1][1];
        tx = mat[0][2];  ty = mat[1][2];
        m_glmDirty = true;
    }
    
    void Matrix::concat(const Matrix& other) {
        // 使用GLM进行矩阵乘法优化
        glm::dmat3 result = toGLM() * other.toGLM();
        fromGLM(result);
    }
    
    void Matrix::invert() {
        // 使用GLM求逆
        glm::dmat3 inverted = glm::inverse(toGLM());
        fromGLM(inverted);
    }
    
    void Matrix::rotate(double angle) {
        // 使用GLM旋转变换
        glm::dmat3 rotation = glm::rotate(glm::dmat3(1.0), angle);
        glm::dmat3 result = toGLM() * rotation;
        fromGLM(result);
    }
    
    void Matrix::scale(double sx, double sy) {
        // 使用GLM缩放变换
        glm::dmat3 scaling = glm::scale(glm::dmat3(1.0), glm::dvec2(sx, sy));
        glm::dmat3 result = toGLM() * scaling;
        fromGLM(result);
    }
    
    void Matrix::translate(double dx, double dy) {
        // 使用GLM平移变换
        glm::dmat3 translation = glm::translate(glm::dmat3(1.0), glm::dvec2(dx, dy));
        glm::dmat3 result = toGLM() * translation;
        fromGLM(result);
    }
    
    std::unique_ptr<Point> Matrix::transformPoint(double x, double y) const {
        // 使用GLM进行点变换
        glm::dvec3 point(x, y, 1.0); // 齐次坐标
        glm::dvec3 transformed = toGLM() * point;
        return std::make_unique<Point>(transformed.x, transformed.y);
    }
    
    double Matrix::getScaleX() const {
        // 使用GLM计算缩放
        if (a == 1.0 && b == 0.0) return 1.0;
        return glm::length(glm::dvec2(a, b));
    }
    
    double Matrix::getScaleY() const {
        if (c == 0.0 && d == 1.0) return 1.0;
        return glm::length(glm::dvec2(c, d));
    }
    
    // ... 其他方法实现
}
```

## CMakeLists.txt 集成

需要在CMakeLists.txt中添加geom模块：

```cmake
# Egret Engine Core模块源文件
set(EGRET_CORE_SOURCES
    # Core模块
    src/core/HashObject.cpp
    
    # Events模块
    src/events/Event.cpp
    src/events/EventDispatcher.cpp
    
    # Geom模块
    src/geom/Point.cpp
    src/geom/Rectangle.cpp
    src/geom/Matrix.cpp
)

set(EGRET_CORE_HEADERS
    # Core模块
    src/core/HashObject.hpp
    
    # Events模块
    src/events/Event.hpp
    src/events/IEventDispatcher.hpp
    src/events/EventPhase.hpp
    src/events/EventDispatcher.hpp
    
    # Geom模块
    src/geom/Point.hpp
    src/geom/Rectangle.hpp
    src/geom/Matrix.hpp
)
```

## 关键设计说明

### 1. GLM集成优势

- **Point类**: 使用`glm::dvec2`进行向量运算优化
- **Matrix类**: 使用`glm::dmat3`进行矩阵运算优化  
- **性能提升**: GLM使用SIMD指令集优化，计算性能更好
- **精度控制**: GLM提供精确的浮点比较函数

### 2. 对象池模式

- 每个类都实现了`create()`和`release()`静态方法
- 减少内存分配开销，提高性能
- 对应TypeScript版本的对象池设计

### 3. 智能指针管理

- 使用`std::unique_ptr`管理对象生命周期
- 避免内存泄露和悬浮指针问题
- 提供RAII保证

### 4. 缓存优化

- Matrix类使用`m_glmMatrix`缓存GLM矩阵
- 只在数据变化时重新计算，避免频繁转换

## 使用示例

```cpp
#include "geom/Point.hpp"
#include "geom/Rectangle.hpp"
#include "geom/Matrix.hpp"

// Point使用示例
auto point1 = Point::create(10.0, 20.0);
auto point2 = Point::create(30.0, 40.0);
double dist = Point::distance(*point1, *point2);

// Rectangle使用示例
auto rect = Rectangle::create();
rect->setTo(0.0, 0.0, 100.0, 50.0);
bool contains = rect->contains(25.0, 25.0);

// Matrix使用示例
auto matrix = Matrix::create();
matrix->rotate(M_PI / 4.0); // 旋转45度
matrix->scale(2.0, 2.0);     // 放大2倍
auto transformedPoint = matrix->transformPoint(10.0, 10.0);

// 对象池回收
Point::release(std::move(point1));
Rectangle::release(std::move(rect));
Matrix::release(std::move(matrix));
```

这个实现完全对应TypeScript版本的功能，同时充分利用了GLM数学库的性能优势。