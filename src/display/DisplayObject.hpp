#pragma once
#include "events/EventDispatcher.hpp"
#include "geom/Matrix.hpp"
#include "geom/Rectangle.hpp"
#include "geom/Point.hpp"
#include "display/DisplayList.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cmath>

namespace egret {
    
    // 前向声明
    class DisplayObjectContainer;
    class Stage;
    
    namespace sys {
        class DisplayList;
        class SystemRenderer;  // SystemRenderer前向声明
        class SkiaRenderer;   // SkiaRenderer前向声明
    }
    
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
        virtual ~DisplayObject();
        
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
        double getX() const { return getXInternal(); }
        virtual void setX(double value) { setXInternal(value); }
        double getXInternal() const { return m_x; }
        bool setXInternal(double value);
        
        /**
         * Y坐标位置  
         */
        double getY() const { return getYInternal(); }
        virtual void setY(double value) { setYInternal(value); }
        double getYInternal() const { return m_y; }
        bool setYInternal(double value);
        
        // ========== 缩放 ==========
        
        /**
         * 水平缩放比例
         */
        double getScaleX() const { return getScaleXInternal(); }
        virtual void setScaleX(double value) { setScaleXInternal(value); }
        double getScaleXInternal() const { return m_scaleX; }
        void setScaleXInternal(double value);
        
        /**
         * 垂直缩放比例
         */
        double getScaleY() const { return getScaleYInternal(); }
        virtual void setScaleY(double value) { setScaleYInternal(value); }
        double getScaleYInternal() const { return m_scaleY; }
        void setScaleYInternal(double value);
        
        // ========== 旋转和倾斜 ==========
        
        /**
         * 旋转角度（度数）
         */
        double getRotation() const { return getRotationInternal(); }
        virtual void setRotation(double value) { setRotationInternal(value); }
        double getRotationInternal() const { return m_rotation; }
        void setRotationInternal(double value);
        
        /**
         * X轴倾斜角度（弧度）
         */
        double getSkewX() const { return getSkewXInternal(); }
        virtual void setSkewX(double value) { setSkewXInternal(value); }
        double getSkewXInternal() const { return m_skewX; }
        void setSkewXInternal(double value);
        
        /**
         * Y轴倾斜角度（弧度）
         */
        double getSkewY() const { return getSkewYInternal(); }
        virtual void setSkewY(double value) { setSkewYInternal(value); }
        double getSkewYInternal() const { return m_skewY; }
        void setSkewYInternal(double value);
        
        // ========== 显示属性 ==========
        
        /**
         * 透明度 (0.0 - 1.0)
         */
        double getAlpha() const { return getAlphaInternal(); }
        virtual void setAlpha(double value) { setAlphaInternal(value); }
        double getAlphaInternal() const { return m_alpha; }
        void setAlphaInternal(double value);
        
        /**
         * 是否可见
         */
        bool getVisible() const { return getVisibleInternal(); }
        virtual void setVisible(bool value) { setVisibleInternal(value); }
        bool getVisibleInternal() const { return m_visible; }
        void setVisibleInternal(bool value);
        
        /**
         * 是否启用触摸
         */
        bool getTouchEnabled() const { return getTouchEnabledInternal(); }
        virtual void setTouchEnabled(bool value) { setTouchEnabledInternal(value); }
        bool getTouchEnabledInternal() const { return m_touchEnabled; }
        void setTouchEnabledInternal(bool value);
        
        // ========== 变换矩阵 ==========
        
        /**
         * 变换矩阵
         */
        Matrix getMatrix();
        virtual void setMatrix(const Matrix& matrix);
        Matrix* getMatrixInternal();
        void setMatrixInternal(const Matrix& matrix, bool needUpdateProperties = true);
        Matrix* getConcatenatedMatrix();
        Matrix* getInvertedConcatenatedMatrix();
        
        // ========== 锚点 ==========
        
        /**
         * 锚点X偏移
         */
        double getAnchorOffsetX() const { return getAnchorOffsetXInternal(); }
        void setAnchorOffsetX(double value) { setAnchorOffsetXInternal(value); }
        double getAnchorOffsetXInternal() const { return m_anchorOffsetX; }
        void setAnchorOffsetXInternal(double value);
        
        /**
         * 锚点Y偏移
         */
        double getAnchorOffsetY() const { return getAnchorOffsetYInternal(); }
        void setAnchorOffsetY(double value) { setAnchorOffsetYInternal(value); }
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
        Rectangle* getScrollRect() const { return getScrollRectInternal(); }
        void setScrollRect(Rectangle* value) { setScrollRectInternal(value); }
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
        DisplayObject* getMask() const { return getMaskInternal(); }
        void setMask(DisplayObject* value) { setMaskInternal(value); }
        DisplayObject* getMaskInternal() const { return m_mask; }
        void setMaskInternal(DisplayObject* value);
        
        /**
         * 颜色调节
         */
        uint32_t getTint() const { return getTintInternal(); }
        void setTint(uint32_t value) { setTintInternal(value); }
        uint32_t getTintInternal() const { return m_tint; }
        void setTintInternal(uint32_t value);
        
        // ========== 坐标转换 ==========
        
        /**
         * 将舞台坐标转换为本地坐标
         */
        Point globalToLocal(double stageX = 0.0, double stageY = 0.0, Point* resultPoint = nullptr);
        
        /**
         * 将本地坐标转换为舞台坐标
         */
        Point localToGlobal(double localX = 0.0, double localY = 0.0, Point* resultPoint = nullptr);
        
        // ========== 碰撞检测 ==========
        
        /**
         * 点碰撞检测
         */
        virtual bool hitTestPoint(double x, double y, bool shapeFlag = false);
        
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
        void onAddToStageInternal(Stage* stage, int nestLevel);
        
        /**
         * 从舞台移除时调用（内部使用）
         */
        void onRemoveFromStageInternal();
        
        /**
         * 添加到舞台时调用（虚函数版本，供子类重写）
         */
        virtual void onAddToStage(Stage* stage, int nestLevel) {
            onAddToStageInternal(stage, nestLevel);
        }
        
        /**
         * 从舞台移除时调用（虚函数版本，供子类重写）
         */
        virtual void onRemoveFromStage() {
            onRemoveFromStageInternal();
        }
        
        /**
         * 测量显示对象尺寸（虚函数，供子类重写）
         */
        virtual void measure() {}
        
        /**
         * 测量内容边界（虚函数，供子类重写）
         * @param bounds 边界矩形，用于接收测量结果
         */
        virtual void measureContentBounds(Rectangle& bounds) {}
        
        /**
         * 碰撞检测（虚函数，供子类重写）
         * @param stageX 舞台X坐标
         * @param stageY 舞台Y坐标
         * @return 碰撞的显示对象，如果没有碰撞返回nullptr
         */
        virtual DisplayObject* hitTest(double stageX, double stageY) { return nullptr; }
        
        /**
         * 获取嵌套深度
         */
        int getNestLevel() const { return m_nestLevel; }
        
        /**
         * 检查是否已添加到舞台
         */
        bool getHasAddToStage() const { return m_hasAddToStage; }
        
        // ========== 边界计算 ==========
        
        /**
         * 获取显示边界 - 匹配TypeScript版本 getBounds(resultRect?: Rectangle, calculateAnchor: boolean = true)
         */
        virtual Rectangle getBounds(Rectangle* resultRect = nullptr, bool calculateAnchor = true);
        
        /**
         * 获取测量边界
         */
        virtual Rectangle getMeasuredBounds();
        
        /**
         * 获取显示边界（返回智能指针版本）
         */
        virtual std::shared_ptr<Rectangle> getBounds() const;
        
        /**
         * 获取测量边界（返回智能指针版本）
         */
        virtual std::shared_ptr<Rectangle> getMeasuredBounds() const;
        
        /**
         * 检查缓存是否脏
         */
        bool isCacheDirty() const { return m_cacheDirty; }
        
        /**
         * 设置缓存脏标记（公共方法）
         */
        void setCacheDirty(bool value) { m_cacheDirty = value; }
        
        /**
         * 向上传播缓存脏标记（公共方法）
         */
        void cacheDirtyUp();
        
    protected:
        // ========== 受保护的辅助方法 ==========
        
        /**
         * 更新变换使用状态
         */
        void updateUseTransform();
        
        /**
         * 检查缓存是否脏（受保护版本）
         */
        bool getCacheDirty() const { return m_cacheDirty; }
        
        /**
         * 检查是否使用变换
         */
        bool getUseTranslate() const { return m_useTranslate; }
        
        /**
         * 被遮罩的对象
         */
        DisplayObject* getMaskedObject() const { return m_maskedObject; }
        void setMaskedObject(DisplayObject* value) { m_maskedObject = value; }
        
        /**
         * 获取显示列表（用于渲染系统）
         */
        std::shared_ptr<sys::DisplayList> getDisplayList() const { return m_displayList; }
        
        /**
         * 设置显示列表（内部使用）
         */
        void setDisplayList(std::shared_ptr<sys::DisplayList> displayList) { m_displayList = displayList; }
        
        /**
         * 获取渲染节点
         */
        std::shared_ptr<sys::RenderNode> getRenderNode() const;
        
        /**
         * 设置渲染节点
         */
        void setRenderNode(std::shared_ptr<sys::RenderNode> node);
        
        /**
         * 设置渲染脏标记
         */
        void setRenderDirty(bool dirty = true);
        
        /**
         * 设置测量尺寸
         */
        void setMeasuredSize(double width, double height);
        
        
        // ========== 友元类声明 ==========
        friend class sys::SystemRenderer;  // 允许SystemRenderer访问受保护成员
        friend class sys::SkiaRenderer;    // 允许SkiaRenderer访问受保护成员
        
        // ========== 受保护的成员变量（供子类访问） ==========
        
        // 舞台相关
        bool m_hasAddToStage = false;
        int m_nestLevel = 0;
        
        // 舞台引用（Stage类需要设置自己的引用）
        Stage* m_stage = nullptr;
        
    private:
        // ========== 私有成员变量 ==========
        
        // 基本属性
        std::string m_name;
        DisplayObjectContainer* m_parent = nullptr;
        
        // 坐标和变换
        double m_x = 0.0;
        double m_y = 0.0;
        double m_scaleX = 1.0;
        double m_scaleY = 1.0;
        double m_rotation = 0.0;
        double m_skewX = 0.0;
        double m_skewY = 0.0;
        double m_skewXdeg = 0.0;
        double m_skewYdeg = 0.0;
        
        // 显示属性
        double m_alpha = 1.0;
        bool m_visible = true;
        bool m_touchEnabled = true;
        
        // 锚点
        double m_anchorOffsetX = 0.0;
        double m_anchorOffsetY = 0.0;
        
        // 变换矩阵
        std::unique_ptr<Matrix> m_matrix;
        bool m_matrixDirty = false;
        bool m_useTranslate = false;
        std::unique_ptr<Matrix> m_concatenatedMatrix;
        std::unique_ptr<Matrix> m_invertedConcatenatedMatrix;
        
        // 显示效果
        Rectangle* m_scrollRect = nullptr;
        int m_blendMode = 0;
        DisplayObject* m_mask = nullptr;
        DisplayObject* m_maskedObject = nullptr;
        uint32_t m_tint = 0xFFFFFF;
        
        // 缓存标记
        bool m_cacheDirty = false;
        
        // 显示列表（用于渲染）
        std::shared_ptr<sys::DisplayList> m_displayList;
        
        // ========== 私有辅助方法 ==========
        
        /**
         * 限制旋转角度到 -180 到 180 度范围内
         */
        static double clampRotation(double value);
        
        /**
         * 更新矩阵脏标记和变换状态
         */
        void markMatrixDirty();
        
        /**
         * 处理属性变更的通用逻辑
         */
        void onPropertyChanged();
    };
    
} // namespace egret