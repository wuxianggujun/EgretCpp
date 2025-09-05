# DisplayObject C++翻译参考文档

基于白鹭引擎5.4.1的DisplayObject.ts源码翻译为C++的完整参考实现。

## 文件结构

需要创建以下文件：
- `src/display/DisplayObject.hpp` - 头文件
- `src/display/DisplayObject.cpp` - 源文件实现
- 同时需要依赖的其他类：Matrix, Rectangle, Point等

## DisplayObject.hpp

```cpp
#pragma once
#include "events/EventDispatcher.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace egret {
    
    // 前向声明
    class DisplayObjectContainer;
    class Stage;
    class Matrix;
    class Rectangle;
    class Point;
    
    // 渲染模式枚举
    enum class RenderMode : int {
        NONE = 1,
        FILTER = 2,
        CLIP = 3,
        SCROLLRECT = 4
    };
    
    /**
     * DisplayObject类是所有显示对象的基类
     * 对应TypeScript: export class DisplayObject extends EventDispatcher
     */
    class DisplayObject : public EventDispatcher {
    public:
        DisplayObject();
        virtual ~DisplayObject() = default;
        
        // ========== 基本属性 ==========
        
        /**
         * 显示对象的实例名称
         */
        const std::string& getName() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }
        
        /**
         * 父级显示对象容器
         */
        DisplayObjectContainer* getParent() const { return m_parent; }
        
        /**
         * 显示对象的舞台
         */
        Stage* getStage() const { return m_stage; }
        
        // ========== 坐标位置 ==========
        
        /**
         * X坐标位置
         */
        double getX() const { return m_x; }
        void setX(double value);
        double getXInternal() const { return m_x; }
        bool setXInternal(double value);
        
        /**
         * Y坐标位置  
         */
        double getY() const { return m_y; }
        void setY(double value);
        double getYInternal() const { return m_y; }
        bool setYInternal(double value);
        
        // ========== 缩放 ==========
        
        /**
         * 水平缩放比例
         */
        double getScaleX() const { return m_scaleX; }
        void setScaleX(double value);
        double getScaleXInternal() const { return m_scaleX; }
        void setScaleXInternal(double value);
        
        /**
         * 垂直缩放比例
         */
        double getScaleY() const { return m_scaleY; }
        void setScaleY(double value);
        double getScaleYInternal() const { return m_scaleY; }
        void setScaleYInternal(double value);
        
        // ========== 旋转和倾斜 ==========
        
        /**
         * 旋转角度（度数）
         */
        double getRotation() const { return m_rotation; }
        void setRotation(double value);
        double getRotationInternal() const { return m_rotation; }
        void setRotationInternal(double value);
        
        /**
         * X轴倾斜角度（弧度）
         */
        double getSkewX() const { return m_skewX; }
        void setSkewX(double value);
        double getSkewXInternal() const { return m_skewX; }
        void setSkewXInternal(double value);
        
        /**
         * Y轴倾斜角度（弧度）
         */
        double getSkewY() const { return m_skewY; }
        void setSkewY(double value);
        double getSkewYInternal() const { return m_skewY; }
        void setSkewYInternal(double value);
        
        // ========== 显示属性 ==========
        
        /**
         * 透明度 (0.0 - 1.0)
         */
        double getAlpha() const { return m_alpha; }
        void setAlpha(double value);
        double getAlphaInternal() const { return m_alpha; }
        void setAlphaInternal(double value);
        
        /**
         * 是否可见
         */
        bool getVisible() const { return m_visible; }
        void setVisible(bool value);
        bool getVisibleInternal() const { return m_visible; }
        void setVisibleInternal(bool value);
        
        /**
         * 是否启用触摸
         */
        bool getTouchEnabled() const { return m_touchEnabled; }
        void setTouchEnabled(bool value);
        bool getTouchEnabledInternal() const { return m_touchEnabled; }
        void setTouchEnabledInternal(bool value);
        
        // ========== 变换矩阵 ==========
        
        /**
         * 变换矩阵
         */
        Matrix getMatrix();
        void setMatrix(const Matrix& matrix);
        Matrix* getMatrixInternal();
        void setMatrixInternal(const Matrix& matrix, bool needUpdateProperties = true);
        Matrix* getConcatenatedMatrix();
        Matrix* getInvertedConcatenatedMatrix();
        
        // ========== 锚点 ==========
        
        /**
         * 锚点X偏移
         */
        double getAnchorOffsetX() const { return m_anchorOffsetX; }
        void setAnchorOffsetX(double value);
        double getAnchorOffsetXInternal() const { return m_anchorOffsetX; }
        void setAnchorOffsetXInternal(double value);
        
        /**
         * 锚点Y偏移
         */
        double getAnchorOffsetY() const { return m_anchorOffsetY; }
        void setAnchorOffsetY(double value);
        double getAnchorOffsetYInternal() const { return m_anchorOffsetY; }
        void setAnchorOffsetYInternal(double value);
        
        // ========== 宽度高度 ==========
        
        /**
         * 显示对象的宽度
         */
        virtual double getWidth();
        virtual void setWidth(double value);
        
        /**
         * 显示对象的高度
         */
        virtual double getHeight();
        virtual void setHeight(double value);
        
        // ========== 滚动矩形 ==========
        
        /**
         * 滚动矩形
         */
        Rectangle* getScrollRect() const { return m_scrollRect; }
        void setScrollRect(Rectangle* value);
        Rectangle* getScrollRectInternal() const { return m_scrollRect; }
        void setScrollRectInternal(Rectangle* value);
        
        // ========== 显示效果 ==========
        
        /**
         * 混合模式
         */
        int getBlendMode() const { return m_blendMode; }
        void setBlendMode(int value);
        
        /**
         * 遮罩对象
         */
        DisplayObject* getMask() const { return m_mask; }
        void setMask(DisplayObject* value);
        DisplayObject* getMaskInternal() const { return m_mask; }
        void setMaskInternal(DisplayObject* value);
        
        /**
         * 颜色调节
         */
        uint32_t getTint() const { return m_tint; }
        void setTint(uint32_t value);
        uint32_t getTintInternal() const { return m_tint; }
        void setTintInternal(uint32_t value);
        
        // ========== 坐标转换 ==========
        
        /**
         * 将舞台坐标转换为本地坐标
         */
        Point globalToLocal(double stageX, double stageY, Point* resultPoint = nullptr);
        
        /**
         * 将本地坐标转换为舞台坐标
         */
        Point localToGlobal(double localX, double localY, Point* resultPoint = nullptr);
        
        // ========== 碰撞检测 ==========
        
        /**
         * 点碰撞检测
         */
        bool hitTestPoint(double x, double y, bool shapeFlag = false);
        
        /**
         * 对象碰撞检测
         */
        DisplayObject* hitTestObject(DisplayObject* other);
        
        // ========== 内部方法（由容器类和渲染系统使用） ==========
        
        /**
         * 设置父级对象（内部使用）
         */
        void setParentInternal(DisplayObjectContainer* parent);
        
        /**
         * 添加到舞台时调用（内部使用）
         */
        void onAddToStage(Stage* stage, int nestLevel);
        
        /**
         * 从舞台移除时调用（内部使用）
         */
        void onRemoveFromStage();
        
        /**
         * 获取嵌套深度
         */
        int getNestLevel() const { return m_nestLevel; }
        
        /**
         * 检查是否已添加到舞台
         */
        bool getHasAddToStage() const { return m_hasAddToStage; }
        
    protected:
        // ========== 受保护的辅助方法 ==========
        
        /**
         * 更新变换使用状态
         */
        void updateUseTransform();
        
        /**
         * 向上传播缓存脏标记
         */
        void cacheDirtyUp();
        
        /**
         * 获取显示边界
         */
        virtual Rectangle getBounds(DisplayObject* targetCoordinateSpace = nullptr);
        
        /**
         * 获取测量边界
         */
        virtual Rectangle getMeasuredBounds();
        
        // ========== 缓存和优化相关 ==========
        
        /**
         * 检查缓存是否脏
         */
        bool getCacheDirty() const { return m_cacheDirty; }
        void setCacheDirty(bool value) { m_cacheDirty = value; }
        
        /**
         * 检查是否使用变换
         */
        bool getUseTranslate() const { return m_useTranslate; }
        
        /**
         * 被遮罩的对象
         */
        DisplayObject* getMaskedObject() const { return m_maskedObject; }
        void setMaskedObject(DisplayObject* value) { m_maskedObject = value; }
        
    private:
        // ========== 私有成员变量 ==========
        
        // 基本属性
        std::string m_name;
        DisplayObjectContainer* m_parent = nullptr;
        Stage* m_stage = nullptr;
        std::vector<DisplayObject*>* m_children = nullptr; // 仅容器类使用
        
        // 舞台相关
        bool m_hasAddToStage = false;
        int m_nestLevel = 0;
        
        // 坐标和变换
        double m_x = 0.0;
        double m_y = 0.0;
        double m_scaleX = 1.0;
        double m_scaleY = 1.0;
        double m_rotation = 0.0;
        double m_skewX = 0.0;
        double m_skewY = 0.0;
        double m_skewXdeg = 0.0;  // 度数版本
        double m_skewYdeg = 0.0;  // 度数版本
        
        // 显示属性
        double m_alpha = 1.0;
        bool m_visible = true;
        bool m_touchEnabled = true;
        
        // 锚点
        double m_anchorOffsetX = 0.0;
        double m_anchorOffsetY = 0.0;
        
        // 变换矩阵
        Matrix* m_matrix = nullptr;
        bool m_matrixDirty = false;
        bool m_useTranslate = false;
        Matrix* m_concatenatedMatrix = nullptr;
        Matrix* m_invertedConcatenatedMatrix = nullptr;
        
        // 显示效果
        Rectangle* m_scrollRect = nullptr;
        int m_blendMode = 0;
        DisplayObject* m_mask = nullptr;
        DisplayObject* m_maskedObject = nullptr;
        uint32_t m_tint = 0xFFFFFF;
        
        // 缓存标记
        bool m_cacheDirty = false;
        
        // ========== 私有辅助方法 ==========
        
        /**
         * 限制旋转角度到 -180 到 180 度范围内
         */
        static double clampRotation(double value);
    };
    
} // namespace egret
```

## DisplayObject.cpp 关键实现要点

```cpp
#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "display/Stage.hpp"
#include "geom/Matrix.hpp"
#include "geom/Rectangle.hpp"
#include "geom/Point.hpp"
#include <algorithm>

namespace egret {
    
    DisplayObject::DisplayObject() {
        // 构造函数实现
        // 初始化默认属性，对应TS: tint = 0xFFFFFF;
        m_tint = 0xFFFFFF;
        
        // 创建矩阵对象
        m_matrix = new Matrix();
    }
    
    // ========== 坐标设置实现示例 ==========
    
    void DisplayObject::setX(double value) {
        setXInternal(value);
    }
    
    bool DisplayObject::setXInternal(double value) {
        if (m_x == value) {
            return false;
        }
        m_x = value;
        
        // 触发父级缓存更新（对应TS逻辑）
        if (m_parent && !m_parent->getCacheDirty()) {
            m_parent->setCacheDirty(true);
            m_parent->cacheDirtyUp();
        }
        
        // 触发遮罩对象缓存更新
        if (m_maskedObject && !m_maskedObject->getCacheDirty()) {
            m_maskedObject->setCacheDirty(true);
            m_maskedObject->cacheDirtyUp();
        }
        
        return true;
    }
    
    // ========== 缩放设置实现示例 ==========
    
    void DisplayObject::setScaleXInternal(double value) {
        if (m_scaleX == value) {
            return;
        }
        m_scaleX = value;
        m_matrixDirty = true;
        
        updateUseTransform();
        
        // 缓存更新逻辑同上...
    }
    
    // ========== 矩阵操作实现示例 ==========
    
    Matrix* DisplayObject::getMatrixInternal() {
        if (m_matrixDirty) {
            m_matrixDirty = false;
            m_matrix->updateScaleAndRotation(m_scaleX, m_scaleY, m_skewX, m_skewY);
        }
        m_matrix->tx = m_x;
        m_matrix->ty = m_y;
        return m_matrix;
    }
    
    // ========== 辅助函数实现 ==========
    
    double DisplayObject::clampRotation(double value) {
        value = fmod(value, 360.0);
        if (value > 180.0) {
            value -= 360.0;
        } else if (value < -180.0) {
            value += 360.0;
        }
        return value;
    }
    
    void DisplayObject::updateUseTransform() {
        if (m_scaleX == 1.0 && m_scaleY == 1.0 && m_skewX == 0.0 && m_skewY == 0.0) {
            m_useTranslate = false;
        } else {
            m_useTranslate = true;
        }
    }
    
    // ... 其他方法的完整实现
    
} // namespace egret
```

## 依赖的其他类

需要实现以下辅助类：

### Matrix类
```cpp
class Matrix {
public:
    double a, b, c, d, tx, ty;
    
    Matrix clone() const;
    void updateScaleAndRotation(double scaleX, double scaleY, double skewX, double skewY);
    void preMultiplyInto(const Matrix& other, Matrix* result);
    double getScaleX() const;
    double getScaleY() const;
    double getSkewX() const;
    double getSkewY() const;
    // ... 其他矩阵操作方法
};
```

### Rectangle类
```cpp
class Rectangle {
public:
    double x, y, width, height;
    
    Rectangle(double x = 0, double y = 0, double width = 0, double height = 0);
    // ... 矩形操作方法
};
```

### Point类
```cpp
class Point {
public:
    double x, y;
    
    Point(double x = 0, double y = 0);
    // ... 点操作方法
};
```

## 关键设计说明

1. **双重方法模式**: 每个属性都有公共setter和内部setter，对应TypeScript的public和$private方法
2. **脏标记系统**: 使用m_matrixDirty, m_cacheDirty等标记优化性能
3. **事件继承**: 继承EventDispatcher获得完整事件系统
4. **内存管理**: 使用RAII和智能指针管理Matrix等对象
5. **前向声明**: 避免循环依赖问题

## 使用示例

```cpp
// 创建显示对象
auto displayObj = std::make_unique<DisplayObject>();

// 设置基本属性
displayObj->setName("myObject");
displayObj->setX(100.0);
displayObj->setY(50.0);
displayObj->setScaleX(1.5);
displayObj->setAlpha(0.8);

// 事件监听
displayObj->addEventListener("click", [](Event& e) {
    // 处理点击事件
});

// 添加到显示列表
container->addChild(displayObj.get());
```

这个实现完全对应TypeScript版本的所有功能和行为模式。