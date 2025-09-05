#pragma once
#include "DisplayObjectContainer.hpp"
#include <string>
#include <functional>

namespace egret {

    // 前向声明
    namespace sys {
        class Screen;
        class DisplayList;
    }

    /**
     * 舞台缩放模式枚举
     */
    namespace StageScaleMode {
        const std::string EXACT_FIT = "exactFit";
        const std::string SHOW_ALL = "showAll";  
        const std::string NO_SCALE = "noScale";
        const std::string NO_BORDER = "noBorder";
        const std::string FIXED_WIDTH = "fixedWidth";
        const std::string FIXED_HEIGHT = "fixedHeight";
    }

    /**
     * 屏幕方向模式枚举
     */
    namespace OrientationMode {
        const std::string AUTO = "auto";
        const std::string PORTRAIT = "portrait";
        const std::string LANDSCAPE = "landscape";
        const std::string LANDSCAPE_FLIPPED = "landscapeFlipped";
    }

    /**
     * Stage类表示显示列表的根
     * 对应TypeScript: export class Stage extends DisplayObjectContainer
     */
    class Stage : public DisplayObjectContainer {
    public:
        Stage();
        virtual ~Stage() = default;
        
        // ========== 帧率控制 ==========
        
        /**
         * 获取并设置舞台的帧速率。帧速率是指每秒显示的帧数。
         * 帧速率的有效范围为每秒 0.01 到 1000 个帧。
         * 注意: 修改任何一个Stage的frameRate属性都会同步修改其他Stage的帧率。
         * @default 30
         */
        double getFrameRate() const { return m_frameRate; }
        void setFrameRate(double value);
        
        // ========== 舞台尺寸 ==========
        
        /**
         * 舞台的当前宽度（以像素为单位）
         */
        double getStageWidth() const { return m_stageWidth; }
        
        /**
         * 设置舞台宽度（公共方法）
         */
        void setStageWidth(double value) { 
            if (m_stageWidth != value) {
                m_stageWidth = value;
                dispatchResizeEvent();
            }
        }
        
        /**
         * 舞台的当前高度（以像素为单位）  
         */
        double getStageHeight() const { return m_stageHeight; }
        
        /**
         * 设置舞台高度（公共方法）
         */
        void setStageHeight(double value) {
            if (m_stageHeight != value) {
                m_stageHeight = value;
                dispatchResizeEvent();
            }
        }
        
        /**
         * 内部设置舞台宽度（供系统调用）
         */
        void setStageWidthInternal(double value) { m_stageWidth = value; }
        
        /**
         * 内部设置舞台高度（供系统调用）
         */
        void setStageHeightInternal(double value) { m_stageHeight = value; }
        
        // ========== 缩放模式 ==========
        
        /**
         * 一个 StageScaleMode 类中指定要使用哪种缩放模式的值
         * @default StageScaleMode.SHOW_ALL
         */
        const std::string& getScaleMode() const { return m_scaleMode; }
        void setScaleMode(const std::string& value);
        
        // ========== 屏幕方向 ==========
        
        /**
         * 屏幕横竖屏显示方式
         * @default OrientationMode.AUTO
         */
        const std::string& getOrientation() const { return m_orientation; }
        void setOrientation(const std::string& value);
        
        // ========== 纹理缩放 ==========
        
        /**
         * 绘制纹理的缩放比率
         * @default 1.0
         */
        double getTextureScaleFactor() const { return m_textureScaleFactor; }
        void setTextureScaleFactor(double value);
        
        // ========== 触摸控制 ==========
        
        /**
         * 设置屏幕同时可以触摸的数量。高于这个数量将不会被触发响应。
         * @default 99
         */
        int getMaxTouches() const { return m_maxTouches; }
        void setMaxTouches(int value);
        
        // ========== 内容尺寸 ==========
        
        /**
         * 设置分辨率尺寸
         * @param width 宽度
         * @param height 高度
         */
        void setContentSize(double width, double height);
        
        // ========== 渲染控制 ==========
        
        /**
         * 调用 invalidate() 方法后，在显示列表下次呈现时，
         * Egret 会向每个已注册侦听 Event.RENDER 事件的显示对象发送一个 Event.RENDER 事件。
         * 每次您希望 Egret 发送 Event.RENDER 事件时，都必须调用 invalidate() 方法。
         */
        void invalidate();
        
        /**
         * 获取渲染无效标记
         */
        bool getInvalidateRenderFlag() const { return m_invalidateRenderFlag; }
        
        /**
         * 设置渲染无效标记（内部使用）
         */
        void setInvalidateRenderFlag(bool value) { m_invalidateRenderFlag = value; }
        
        // ========== 绘制方法 ==========
        
        /**
         * 自动清除并绘制到表面（供渲染系统调用）
         */
        void drawToSurfaceAutoClear();
        
        /**
         * 绘制到表面（供渲染系统调用）
         */
        void drawToSurface();
        
        /**
         * 调整舞台大小（供系统调用）
         */
        void resize(double width, double height);
        
        // ========== 屏幕引用 ==========
        
        /**
         * 获取屏幕引用
         */
        sys::Screen* getScreen() const { return m_screen; }
        
        /**
         * 设置屏幕引用（内部使用）
         */
        void setScreen(sys::Screen* screen) { m_screen = screen; }
        
        // ========== 显示列表相关 ==========
        
        /**
         * 获取显示列表对象（供渲染系统使用）
         */
        std::shared_ptr<sys::DisplayList> getDisplayList() const { return m_displayList; }
        
        /**
         * 设置显示列表对象（内部使用）
         */
        void setDisplayList(std::shared_ptr<sys::DisplayList> displayList) { m_displayList = displayList; }
        
        // ========== 重写父类方法 ==========
        
        /**
         * Stage对象不能设置某些属性，这些方法会抛出异常或忽略设置
         */
        void setX(double value) override { /* Stage的x位置固定为0 */ }
        void setY(double value) override { /* Stage的y位置固定为0 */ }
        void setScaleX(double value) override { /* Stage不能缩放 */ }
        void setScaleY(double value) override { /* Stage不能缩放 */ }
        void setRotation(double value) override { /* Stage不能旋转 */ }
        void setAlpha(double value) override { /* Stage的alpha固定为1 */ }
        void setVisible(bool value) override { /* Stage的visible固定为true */ }
        void setTouchEnabled(bool value) override { /* Stage的touchEnabled固定为true */ }
        void setMatrix(const Matrix& matrix) override { /* Stage不能设置变换矩阵 */ }
        
    private:
        // ========== 私有成员变量 ==========
        
        // 帧率
        static double s_globalFrameRate;  // 全局帧率
        double m_frameRate = 30.0;
        
        // 舞台尺寸
        double m_stageWidth = 640.0;
        double m_stageHeight = 480.0;
        
        // 缩放和方向
        std::string m_scaleMode = StageScaleMode::SHOW_ALL;
        std::string m_orientation = OrientationMode::AUTO;
        
        // 纹理和触摸
        double m_textureScaleFactor = 1.0;
        int m_maxTouches = 99;
        
        // 渲染控制
        bool m_invalidateRenderFlag = false;
        
        // 系统引用
        sys::Screen* m_screen = nullptr;
        std::shared_ptr<sys::DisplayList> m_displayList = nullptr;  // 显示列表渲染系统
        
        // ========== 私有辅助方法 ==========
        
        /**
         * 更新屏幕尺寸（内部调用）
         */
        void updateScreenSize();
        
        /**
         * 更新最大触摸点数（内部调用）
         */
        void updateMaxTouches();
        
        /**
         * 分发RESIZE事件
         */
        void dispatchResizeEvent();
    };

} // namespace egret