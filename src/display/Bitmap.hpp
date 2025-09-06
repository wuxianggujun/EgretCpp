//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include "display/DisplayObject.hpp"
#include <memory>
#include <string>

namespace egret
{
    // 前向声明
    class Texture;
    class BitmapData;
    
    namespace sys
    {
        class NormalBitmapNode;
    }
    
    /**
     * Bitmap类表示用于显示位图图片的显示对象。
     * 利用Bitmap()构造函数，可以创建包含对BitmapData对象引用的Bitmap对象。
     * 创建了Bitmap对象后，使用父级DisplayObjectContainer实例的addChild()或addChildAt()方法可以将位图放在显示列表中。
     * 一个Bitmap对象可在若干Bitmap对象之中共享其texture引用，与缩放或旋转属性无关。
     * 由于能够创建引用相同texture对象的多个Bitmap对象，因此，多个显示对象可以使用相同的texture对象，
     * 而不会因为每个显示对象实例使用一个texture对象而产生额外内存开销。
     * @version Egret 2.4
     * @platform Web,Native
     */
    class Bitmap : public DisplayObject
    {
    public:
        // ========== 静态常量 ==========
        
        /**
         * 全局默认平滑设置
         */
        static bool defaultSmoothing;
        
        // ========== 构造函数 ==========
        
        /**
         * 创建一个引用指定Texture实例的Bitmap对象
         * @param value 被引用的Texture实例
         * @version Egret 2.4
         * @platform Web,Native
         */
        explicit Bitmap(std::shared_ptr<Texture> value = nullptr);
        
        /**
         * 虚析构函数
         */
        virtual ~Bitmap() = default;
        
        // ========== 公开属性访问器 ==========
        
        /**
         * 被引用的Texture对象
         * 如果传入构造函数的类型为BitmapData或者最后设置的为bitmapData，则此值返回null
         * @version Egret 2.4
         * @platform Web,Native
         */
        std::shared_ptr<Texture> getTexture() const { return m_texture; }
        
        /**
         * 设置纹理
         * @param value 纹理对象
         */
        void setTexture(std::shared_ptr<Texture> value);
        
        /**
         * 被引用的BitmapData对象
         * @version Egret 2.4
         * @platform Web,Native
         */
        std::shared_ptr<BitmapData> getBitmapData() const { return m_bitmapData; }
        
        /**
         * 设置位图数据
         * @param value 位图数据对象
         */
        void setBitmapData(std::shared_ptr<BitmapData> value);
        
        /**
         * 是否对位图应用平滑处理
         * @version Egret 2.4
         * @platform Web,Native
         */
        bool getSmoothing() const { return m_smoothing; }
        
        /**
         * 设置平滑处理
         * @param value 是否平滑
         */
        void setSmoothing(bool value);
        
        /**
         * 设置或获取位图的像素锁定状态
         * @version Egret 2.4
         * @platform Web,Native
         */
        bool getPixelSnapping() const { return m_pixelSnapping; }
        
        /**
         * 设置像素锁定
         * @param value 是否像素锁定
         */
        void setPixelSnapping(bool value) { m_pixelSnapping = value; }
        
        /**
         * 九宫格缩放区域
         * @version Egret 2.4
         * @platform Web,Native
         */
        std::shared_ptr<Rectangle> getScale9Grid() const { return m_scale9Grid; }
        
        /**
         * 设置九宫格缩放区域
         * @param value 缩放区域
         */
        void setScale9Grid(std::shared_ptr<Rectangle> value);
        
        /**
         * 设置Bitmap显示尺寸
         * @param width 宽度
         * @param height 高度
         */
        void setBitmapSize(double width, double height);
        
        // ========== DisplayObject虚函数重写 ==========
        
        /**
         * 计算显示对象的边界
         */
        virtual std::shared_ptr<Rectangle> getBounds() const override;
        
        /**
         * 获取测量边界
         */
        virtual std::shared_ptr<Rectangle> getMeasuredBounds() const override;
        
        // ========== 内部方法（供显示系统使用）==========
        
        /**
         * 刷新图像数据（内部使用）
         */
        void refreshImageData();
        
        /**
         * 设置图像数据（内部使用）
         * @param bitmapData 位图数据
         * @param bitmapX 位图X坐标
         * @param bitmapY 位图Y坐标
         * @param bitmapWidth 位图宽度
         * @param bitmapHeight 位图高度
         * @param offsetX 偏移X
         * @param offsetY 偏移Y
         * @param textureWidth 纹理宽度
         * @param textureHeight 纹理高度
         * @param sourceWidth 源宽度
         * @param sourceHeight 源高度
         */
        void setImageData(std::shared_ptr<BitmapData> bitmapData, double bitmapX, double bitmapY, 
                         double bitmapWidth, double bitmapHeight, double offsetX, double offsetY,
                         double textureWidth, double textureHeight, double sourceWidth, double sourceHeight);

        /**
         * 为渲染准备渲染节点数据（在渲染前调用）
         * 根据当前纹理、scale9Grid、显式尺寸等，更新 NormalBitmapNode 的 drawData。
         */
        void prepareRenderNode();
        
    protected:
        // ========== DisplayObject虚函数重写 ==========
        
        /**
         * 添加到舞台时的处理
         */
        void onAddToStage(Stage* stage, int nestLevel) override;
        
        /**
         * 从舞台移除时的处理
         */
        void onRemoveFromStage() override;
        
        /**
         * 测量显示对象尺寸
         */
        virtual void measure() override;
        
    private:
        // ========== 私有成员变量 ==========
        
        std::shared_ptr<Texture> m_texture;              // 纹理对象
        std::shared_ptr<BitmapData> m_bitmapData;        // 位图数据对象
        double m_bitmapX;                                // 位图X坐标
        double m_bitmapY;                                // 位图Y坐标
        double m_bitmapWidth;                            // 位图宽度
        double m_bitmapHeight;                           // 位图高度
        double m_offsetX;                                // 偏移X
        double m_offsetY;                                // 偏移Y
        double m_textureWidth;                           // 纹理宽度
        double m_textureHeight;                          // 纹理高度
        double m_sourceWidth;                            // 源宽度
        double m_sourceHeight;                           // 源高度
        bool m_smoothing;                                // 平滑处理
        bool m_pixelSnapping;                            // 像素锁定
        double m_explicitBitmapWidth;                    // 明确的位图宽度
        double m_explicitBitmapHeight;                   // 明确的位图高度
        std::shared_ptr<Rectangle> m_scale9Grid;         // 九宫格缩放区域
        
        // ========== 私有辅助方法 ==========
        
        /**
         * 设置纹理的内部实现
         * @param value 纹理对象
         * @return 是否发生了变化
         */
        bool setTextureInternal(std::shared_ptr<Texture> value);
        
        /**
         * 标记渲染脏标记并更新父级
         */
        void markRenderDirty();
    };
    
} // namespace egret
