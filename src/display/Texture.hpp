//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include "core/HashObject.hpp"
#include <memory>
#include <cstdint>
#include <vector>
#include <string>

namespace egret
{
    // 前向声明
    class BitmapData;
    class Rectangle;
    
    /**
     * 全局纹理缩放因子
     */
    extern double TextureScaleFactor;
    
    /**
     * 纹理类是对不同平台不同的图片资源的封装
     * 在HTML5中，资源是一个HTMLElement对象
     * 在OpenGL/WebGL中，资源是一个提交GPU后获取的纹理id
     * Texture类封装了这些底层实现的细节，开发者只需要关心接口即可
     * @version Egret 2.4
     * @platform Web,Native
     */
    class Texture : public HashObject
    {
    public:
        // ========== 构造函数 ==========
        
        /**
         * 创建一个Texture对象
         * @version Egret 2.4
         * @platform Web,Native
         */
        Texture();
        
        /**
         * 虚析构函数
         */
        virtual ~Texture() = default;
        
        // ========== 公开属性访问器 ==========
        
        /**
         * 销毁纹理时是否销毁对应BitmapData
         * @version Egret 5.0.8
         * @platform Web,Native
         */
        bool getDisposeBitmapData() const { return m_disposeBitmapData; }
        void setDisposeBitmapData(bool value) { m_disposeBitmapData = value; }
        
        /**
         * 被引用的BitmapData对象
         * @version Egret 2.4
         * @platform Web,Native
         */
        std::shared_ptr<BitmapData> getBitmapData() const { return m_bitmapData; }
        void setBitmapData(std::shared_ptr<BitmapData> value);
        
        /**
         * 纹理在BitmapData上的X起始位置
         */
        double getBitmapX() const { return m_bitmapX; }
        
        /**
         * 纹理在BitmapData上的Y起始位置
         */
        double getBitmapY() const { return m_bitmapY; }
        
        /**
         * 纹理在BitmapData上的宽度
         */
        double getBitmapWidth() const { return m_bitmapWidth; }
        
        /**
         * 纹理在BitmapData上的高度
         */
        double getBitmapHeight() const { return m_bitmapHeight; }
        
        /**
         * 纹理在X轴上的偏移量
         */
        double getOffsetX() const { return m_offsetX; }
        
        /**
         * 纹理在Y轴上的偏移量
         */
        double getOffsetY() const { return m_offsetY; }
        
        /**
         * 纹理的显示宽度
         */
        double getTextureWidth() const { return m_textureWidth; }
        
        /**
         * 纹理的显示高度
         */
        double getTextureHeight() const { return m_textureHeight; }
        
        /**
         * 纹理的源宽度
         */
        double getSourceWidth() const { return m_sourceWidth; }
        
        /**
         * 纹理的源高度
         */
        double getSourceHeight() const { return m_sourceHeight; }
        
        /**
         * 纹理是否旋转
         */
        bool isRotated() const { return m_rotated; }
        
        // ========== 公开方法 ==========
        
        /**
         * 初始化纹理数据
         * @param bitmapX 纹理在位图上的X坐标
         * @param bitmapY 纹理在位图上的Y坐标
         * @param bitmapWidth 纹理在位图上的宽度
         * @param bitmapHeight 纹理在位图上的高度
         * @param offsetX X轴偏移
         * @param offsetY Y轴偏移
         * @param textureWidth 纹理显示宽度
         * @param textureHeight 纹理显示高度
         * @param sourceWidth 源宽度
         * @param sourceHeight 源高度
         * @param rotated 是否旋转
         */
        void initData(double bitmapX, double bitmapY, double bitmapWidth, double bitmapHeight,
                     double offsetX, double offsetY, double textureWidth, double textureHeight,
                     double sourceWidth, double sourceHeight, bool rotated = false);
        
        /**
         * 获取指定像素区域的颜色值
         * @param x 像素区域的X坐标
         * @param y 像素区域的Y坐标
         * @param width 像素区域的宽度
         * @param height 像素区域的高度
         * @return 像素区域的颜色值数组
         * @version Egret 3.2.1
         * @platform Web,Native
         */
        std::vector<uint32_t> getPixels(int x, int y, int width = 1, int height = 1) const;
        
        /**
         * 转换纹理为base64字符串
         * @param type 图片类型，默认为"image/png"
         * @param encoderOptions 编码选项
         * @return base64字符串
         * @version Egret 3.2.1
         * @platform Web,Native
         */
        std::string toDataURL(const std::string& type = "image/png", double encoderOptions = 0.92) const;
        
        /**
         * 裁剪纹理，返回一个新的Texture实例
         * @param rect 裁剪区域
         * @param offsetX 偏移X
         * @param offsetY 偏移Y
         * @return 新的Texture实例
         * @version Egret 2.4
         * @platform Web,Native
         */
        std::shared_ptr<Texture> crop(std::shared_ptr<Rectangle> rect, double offsetX = 0, double offsetY = 0) const;
        
        /**
         * 销毁纹理
         * @version Egret 5.0.8
         * @platform Web,Native
         */
        void dispose();
        
        // ========== 静态工厂方法 ==========
        
        /**
         * 从BitmapData创建Texture
         * @param bitmapData BitmapData对象
         * @param offsetX X偏移
         * @param offsetY Y偏移
         * @param width 宽度
         * @param height 高度
         * @return Texture实例
         */
        static std::shared_ptr<Texture> createFromBitmapData(std::shared_ptr<BitmapData> bitmapData, 
                                                             double offsetX = 0, double offsetY = 0,
                                                             double width = 0, double height = 0);
        
        /**
         * 创建空纹理
         * @param width 宽度
         * @param height 高度
         * @return Texture实例
         */
        static std::shared_ptr<Texture> createEmpty(double width, double height);
        
    protected:
        /**
         * 设置BitmapData对象的内部实现
         * @param value BitmapData对象
         */
        void setBitmapDataInternal(std::shared_ptr<BitmapData> value);
        
    private:
        // ========== 私有成员变量 ==========
        
        bool m_disposeBitmapData;                         // 销毁纹理时是否销毁BitmapData
        std::shared_ptr<BitmapData> m_bitmapData;         // 被引用的BitmapData对象
        double m_bitmapX;                                 // 纹理在位图上的X起始位置
        double m_bitmapY;                                 // 纹理在位图上的Y起始位置
        double m_bitmapWidth;                             // 纹理在位图上的宽度
        double m_bitmapHeight;                            // 纹理在位图上的高度
        double m_offsetX;                                 // X轴偏移量
        double m_offsetY;                                 // Y轴偏移量
        double m_textureWidth;                            // 纹理的显示宽度
        double m_textureHeight;                           // 纹理的显示高度
        double m_sourceWidth;                             // 纹理的源宽度
        double m_sourceHeight;                            // 纹理的源高度
        bool m_rotated;                                   // 纹理是否旋转
    };
    
} // namespace egret