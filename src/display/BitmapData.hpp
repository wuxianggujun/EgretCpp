//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include "core/HashObject.hpp"
#include <memory>
#include <vector>
#include <cstdint>
#include <string>

namespace egret
{
    // 前向声明
    class DisplayObject;
    class Rectangle;
    class ImageLoader;  // 前向声明ImageLoader
    
    /**
     * 压缩纹理数据
     */
    struct CompressedTextureData
    {
        uint32_t glInternalFormat;    // OpenGL内部格式
        int width;                    // 宽度
        int height;                   // 高度
        std::vector<uint8_t> byteArray; // 字节数据
        int face;                     // 面索引
        int level;                    // 级别
    };
    
    /**
     * BitmapData对象是一个包含像素数据的数组。此数据可以表示完全不透明的位图，或表示包含Alpha通道数据的透明位图。
     * 以上任一类型的BitmapData对象都作为32位整数的缓冲区进行存储。每个32位整数确定位图中单个像素的属性。
     * 每个32位整数都是四个8位通道值（从0到255）的组合，这些值描述像素的Alpha透明度以及红色、绿色、蓝色(ARGB)值。
     * (对于ARGB值，最高有效字节代表Alpha通道值，其后的有效字节分别代表红色、绿色和蓝色通道值。)
     * @version Egret 2.4
     * @platform Web,Native
     */
    class BitmapData : public HashObject, public std::enable_shared_from_this<BitmapData>
    {
        // 友元声明
        friend class ImageLoader;  // 允许ImageLoader访问私有成员
        
    public:
        // ========== 构造函数 ==========
        
        /**
         * 创建一个引用指定数据的BitmapData对象
         * @param data 图像数据源（可以为空）
         */
        explicit BitmapData(void* data = nullptr);
        
        /**
         * 虚析构函数
         */
        virtual ~BitmapData();
        
        // ========== 公开属性访问器 ==========
        
        /**
         * 位图图像的宽度，以像素为单位
         * @readOnly
         * @version Egret 2.4
         * @platform Web,Native
         */
        int getWidth() const { return m_width; }
        
        /**
         * 位图图像的高度，以像素为单位
         * @readOnly
         * @version Egret 2.4
         * @platform Web,Native
         */
        int getHeight() const { return m_height; }
        
        /**
         * 图像数据格式，用于标识图像类型
         * @version Egret 5.2.21
         * @platform Web,Native
         */
        const std::string& getFormat() const { return m_format; }
        void setFormat(const std::string& value) { m_format = value; }
        
        // ========== 公开方法 ==========
        
        /**
         * 获取指定像素的32位ARGB颜色值
         * @param x 像素的X坐标
         * @param y 像素的Y坐标
         * @return 表示像素颜色的32位ARGB值
         * @version Egret 2.4
         * @platform Web,Native
         */
        uint32_t getPixel(int x, int y) const;
        
        /**
         * 获取指定像素的32位ARGB颜色值（包含Alpha通道）
         * @param x 像素的X坐标
         * @param y 像素的Y坐标
         * @return 表示像素颜色的32位ARGB值
         * @version Egret 2.4
         * @platform Web,Native
         */
        uint32_t getPixel32(int x, int y) const;
        
        /**
         * 设置单个像素的颜色
         * @param x 像素的X坐标
         * @param y 像素的Y坐标
         * @param color 像素的RGB颜色值
         * @version Egret 2.4
         * @platform Web,Native
         */
        void setPixel(int x, int y, uint32_t color);
        
        /**
         * 设置单个像素的颜色和Alpha值
         * @param x 像素的X坐标
         * @param y 像素的Y坐标
         * @param color 像素的ARGB颜色值
         * @version Egret 2.4
         * @platform Web,Native
         */
        void setPixel32(int x, int y, uint32_t color);
        
        /**
         * 获取指定区域的像素数据
         * @param x 区域左上角的X坐标
         * @param y 区域左上角的Y坐标
         * @param width 区域宽度
         * @param height 区域高度
         * @return 像素数据数组
         * @version Egret 3.2.1
         * @platform Web,Native
         */
        std::vector<uint32_t> getPixels(int x, int y, int width = 1, int height = 1) const;
        
        /**
         * 设置指定区域的像素数据
         * @param x 区域左上角的X坐标
         * @param y 区域左上角的Y坐标
         * @param width 区域宽度
         * @param height 区域高度
         * @param pixels 像素数据数组
         * @version Egret 3.2.1
         * @platform Web,Native
         */
        void setPixels(int x, int y, int width, int height, const std::vector<uint32_t>& pixels);
        
        /**
         * 转换位图数据为base64字符串
         * @param type 图片类型，默认为"image/png"
         * @param encoderOptions 编码选项
         * @return base64字符串
         * @version Egret 3.2.1
         * @platform Web,Native
         */
        std::string toDataURL(const std::string& type = "image/png", double encoderOptions = 0.92) const;
        
        /**
         * 获取指定区域的像素数据作为新的BitmapData
         * @param rect 指定的矩形区域
         * @return 新的BitmapData实例
         * @version Egret 2.4
         * @platform Web,Native
         */
        std::shared_ptr<BitmapData> crop(std::shared_ptr<Rectangle> rect) const;
        
        /**
         * 复制指定区域的像素数据到另一个BitmapData
         * @param sourceBitmapData 源BitmapData
         * @param sourceRect 源区域
         * @param destPoint 目标位置
         * @version Egret 2.4
         * @platform Web,Native
         */
        void copyPixels(std::shared_ptr<BitmapData> sourceBitmapData, 
                       std::shared_ptr<Rectangle> sourceRect,
                       std::shared_ptr<Rectangle> destPoint) const;
        
        /**
         * 填充指定区域
         * @param rect 要填充的区域
         * @param color 填充颜色
         * @version Egret 2.4
         * @platform Web,Native
         */
        void fillRect(std::shared_ptr<Rectangle> rect, uint32_t color);
        
        /**
         * 销毁位图数据
         * @version Egret 5.0.8
         * @platform Web,Native
         */
        void dispose();
        
        // ========== 静态工厂方法 ==========
        
        /**
         * 创建一个新的BitmapData对象
         * @param width 位图宽度
         * @param height 位图高度
         * @param transparent 是否支持透明度
         * @param fillColor 填充颜色（ARGB格式）
         * @return 新的BitmapData实例
         * @version Egret 2.4
         * @platform Web,Native
         */
        static std::shared_ptr<BitmapData> create(int width, int height, bool transparent = true, uint32_t fillColor = 0xFFFFFFFF);
        
        /**
         * 从图像数据创建BitmapData
         * @param imageData 图像数据
         * @param width 宽度
         * @param height 高度
         * @return BitmapData实例
         */
        static std::shared_ptr<BitmapData> createFromImageData(const std::vector<uint8_t>& imageData, int width, int height);
        
        // ========== 静态管理方法 ==========
        
        /**
         * 添加显示对象到BitmapData管理器（内部使用）
         */
        static void addDisplayObject(std::shared_ptr<DisplayObject> displayObject, std::shared_ptr<BitmapData> bitmapData);
        
        /**
         * 从BitmapData管理器移除显示对象（内部使用）
         */
        static void removeDisplayObject(std::shared_ptr<DisplayObject> displayObject, std::shared_ptr<BitmapData> bitmapData);
        
        /**
         * 标记BitmapData需要更新（内部使用）
         */
        static void invalidate(std::shared_ptr<BitmapData> bitmapData);
        
    protected:
        // ========== 受保护的辅助方法 ==========
        
        /**
         * 确保坐标在有效范围内
         */
        bool isValidCoordinate(int x, int y) const;
        
        /**
         * 获取像素数据指针
         */
        uint32_t* getPixelData() const { return m_pixelData.get(); }
        
    private:
        // ========== 私有成员变量 ==========
        
        int m_width;                                      // 位图宽度
        int m_height;                                     // 位图高度
        std::string m_format;                             // 图像格式
        std::unique_ptr<uint32_t[]> m_pixelData;         // 像素数据
        bool m_disposed;                                  // 是否已销毁
        
        // ========== 私有辅助方法 ==========
        
        /**
         * 分配像素数据内存
         */
        void allocatePixelData();
        
        /**
         * 释放像素数据内存
         */
        void deallocatePixelData();
        
        /**
         * 初始化BitmapData
         */
        void initialize(int width, int height, bool transparent, uint32_t fillColor);
    };
    
} // namespace egret