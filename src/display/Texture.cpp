//
// Created by wuxianggujun on 25-9-4.
//

#include "Texture.hpp"
#include "BitmapData.hpp"
#include "geom/Rectangle.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace egret
{
    // ========== 全局变量定义 ==========
    double TextureScaleFactor = 1.0;
    
    // ========== 构造函数 ==========
    Texture::Texture()
        : HashObject()
        , m_disposeBitmapData(true)
        , m_bitmapData(nullptr)
        , m_bitmapX(0.0)
        , m_bitmapY(0.0)
        , m_bitmapWidth(0.0)
        , m_bitmapHeight(0.0)
        , m_offsetX(0.0)
        , m_offsetY(0.0)
        , m_textureWidth(0.0)
        , m_textureHeight(0.0)
        , m_sourceWidth(0.0)
        , m_sourceHeight(0.0)
        , m_rotated(false)
    {
    }
    
    // ========== 公开属性访问器实现 ==========
    
    void Texture::setBitmapData(std::shared_ptr<BitmapData> value)
    {
        setBitmapDataInternal(value);
    }
    
    // ========== 公开方法实现 ==========
    
    void Texture::initData(double bitmapX, double bitmapY, double bitmapWidth, double bitmapHeight,
                          double offsetX, double offsetY, double textureWidth, double textureHeight,
                          double sourceWidth, double sourceHeight, bool rotated)
    {
        double scale = TextureScaleFactor;
        m_bitmapX = bitmapX / scale;
        m_bitmapY = bitmapY / scale;
        m_bitmapWidth = bitmapWidth / scale;
        m_bitmapHeight = bitmapHeight / scale;
        
        m_offsetX = offsetX;
        m_offsetY = offsetY;
        m_textureWidth = textureWidth;
        m_textureHeight = textureHeight;
        
        m_sourceWidth = sourceWidth;
        m_sourceHeight = sourceHeight;
        
        m_rotated = rotated;
        
        // 通知BitmapData需要更新
        if (m_bitmapData) {
            // BitmapData::invalidate(m_bitmapData);
        }
    }
    
    std::vector<uint32_t> Texture::getPixels(int x, int y, int width, int height) const
    {
        std::vector<uint32_t> pixels;
        
        if (m_bitmapData) {
            // 调整坐标到纹理内的相对坐标
            int adjustedX = static_cast<int>(m_bitmapX) + x;
            int adjustedY = static_cast<int>(m_bitmapY) + y;
            
            // 确保不超出纹理边界
            int actualWidth = std::min(width, static_cast<int>(m_bitmapWidth) - x);
            int actualHeight = std::min(height, static_cast<int>(m_bitmapHeight) - y);
            
            if (actualWidth > 0 && actualHeight > 0) {
                pixels = m_bitmapData->getPixels(adjustedX, adjustedY, actualWidth, actualHeight);
            }
        }
        
        return pixels;
    }
    
    std::string Texture::toDataURL(const std::string& type, double encoderOptions) const
    {
        if (m_bitmapData) {
            return m_bitmapData->toDataURL(type, encoderOptions);
        }
        return "";
    }
    
    std::shared_ptr<Texture> Texture::crop(std::shared_ptr<Rectangle> rect, double offsetX, double offsetY) const
    {
        if (!rect || !m_bitmapData) {
            return nullptr;
        }
        
        auto newTexture = std::make_shared<Texture>();
        
        // 计算新的纹理参数
        double newBitmapX = m_bitmapX + rect->getX();
        double newBitmapY = m_bitmapY + rect->getY();
        double newBitmapWidth = rect->getWidth();
        double newBitmapHeight = rect->getHeight();
        
        double newOffsetX = m_offsetX + offsetX;
        double newOffsetY = m_offsetY + offsetY;
        
        // 初始化新纹理
        newTexture->setBitmapDataInternal(m_bitmapData);
        newTexture->initData(newBitmapX, newBitmapY, newBitmapWidth, newBitmapHeight,
                            newOffsetX, newOffsetY, rect->getWidth(), rect->getHeight(),
                            rect->getWidth(), rect->getHeight(), m_rotated);
        
        return newTexture;
    }
    
    void Texture::dispose()
    {
        if (m_bitmapData && m_disposeBitmapData) {
            m_bitmapData->dispose();
        }
        m_bitmapData = nullptr;
        
        // 重置所有参数
        m_bitmapX = 0.0;
        m_bitmapY = 0.0;
        m_bitmapWidth = 0.0;
        m_bitmapHeight = 0.0;
        m_offsetX = 0.0;
        m_offsetY = 0.0;
        m_textureWidth = 0.0;
        m_textureHeight = 0.0;
        m_sourceWidth = 0.0;
        m_sourceHeight = 0.0;
        m_rotated = false;
    }
    
    // ========== 静态工厂方法实现 ==========
    
    std::shared_ptr<Texture> Texture::createFromBitmapData(std::shared_ptr<BitmapData> bitmapData,
                                                           double offsetX, double offsetY,
                                                           double width, double height)
    {
        if (!bitmapData) {
            return nullptr;
        }
        
        auto texture = std::make_shared<Texture>();
        
        // 如果没有指定宽高，使用BitmapData的完整尺寸
        double actualWidth = (width > 0) ? width : bitmapData->getWidth();
        double actualHeight = (height > 0) ? height : bitmapData->getHeight();
        
        texture->setBitmapDataInternal(bitmapData);
        texture->initData(0, 0, actualWidth, actualHeight,
                         offsetX, offsetY, actualWidth, actualHeight,
                         actualWidth, actualHeight, false);
        
        return texture;
    }
    
    std::shared_ptr<Texture> Texture::createEmpty(double width, double height)
    {
        auto texture = std::make_shared<Texture>();
        
        // 创建空的BitmapData
        auto bitmapData = BitmapData::create(static_cast<int>(width), static_cast<int>(height), true, 0x00000000);
        
        texture->setBitmapDataInternal(bitmapData);
        texture->initData(0, 0, width, height,
                         0, 0, width, height,
                         width, height, false);
        
        return texture;
    }
    
    // ========== 受保护的方法实现 ==========
    
    void Texture::setBitmapDataInternal(std::shared_ptr<BitmapData> value)
    {
        m_bitmapData = value;
        
        if (value) {
            double scale = TextureScaleFactor;
            double w = value->getWidth() * scale;
            double h = value->getHeight() * scale;
            initData(0, 0, w, h, 0, 0, w, h, value->getWidth(), value->getHeight());
        }
    }
    
} // namespace egret