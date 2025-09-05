//
// Created by wuxianggujun on 25-9-4.
//

#include "BitmapData.hpp"
#include "DisplayObject.hpp"
#include "geom/Rectangle.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <set>
#include <map>

namespace egret
{
    // ========== 静态成员变量 ==========
    // 自定义 weak_ptr 比较器
    struct WeakPtrCompare {
        bool operator()(const std::weak_ptr<DisplayObject>& lhs,
                       const std::weak_ptr<DisplayObject>& rhs) const {
            auto lPtr = lhs.lock();
            auto rPtr = rhs.lock();
            return lPtr.get() < rPtr.get();
        }
    };
    
    static std::map<std::shared_ptr<BitmapData>, std::set<std::weak_ptr<DisplayObject>, WeakPtrCompare>> s_bitmapDataDisplayObjects;
    
    // ========== 构造函数 ==========
    BitmapData::BitmapData(void* data)
        : HashObject()
        , m_width(0)
        , m_height(0)
        , m_format("rgba")
        , m_pixelData(nullptr)
        , m_disposed(false)
    {
        // 如果提供了外部数据，这里可以进行初始化
        // 根据TypeScript版本，主要通过source参数设置width和height
        // C++版本中，ImageLoader将通过友元访问直接设置这些值
    }
    
    BitmapData::~BitmapData()
    {
        if (!m_disposed) {
            dispose();
        }
    }
    
    // ========== 公开方法实现 ==========
    
    uint32_t BitmapData::getPixel(int x, int y) const
    {
        if (!isValidCoordinate(x, y) || !m_pixelData) {
            return 0;
        }
        
        uint32_t pixel = m_pixelData[y * m_width + x];
        return pixel & 0x00FFFFFF; // 移除Alpha通道
    }
    
    uint32_t BitmapData::getPixel32(int x, int y) const
    {
        if (!isValidCoordinate(x, y) || !m_pixelData) {
            return 0;
        }
        
        return m_pixelData[y * m_width + x];
    }
    
    void BitmapData::setPixel(int x, int y, uint32_t color)
    {
        if (!isValidCoordinate(x, y) || !m_pixelData) {
            return;
        }
        
        uint32_t& pixel = m_pixelData[y * m_width + x];
        pixel = (pixel & 0xFF000000) | (color & 0x00FFFFFF); // 保持Alpha，更新RGB
    }
    
    void BitmapData::setPixel32(int x, int y, uint32_t color)
    {
        if (!isValidCoordinate(x, y) || !m_pixelData) {
            return;
        }
        
        m_pixelData[y * m_width + x] = color;
    }
    
    std::vector<uint32_t> BitmapData::getPixels(int x, int y, int width, int height) const
    {
        std::vector<uint32_t> pixels;
        
        if (!m_pixelData) {
            return pixels;
        }
        
        // 确保区域在边界内
        int startX = std::max(0, x);
        int startY = std::max(0, y);
        int endX = std::min(m_width, x + width);
        int endY = std::min(m_height, y + height);
        
        if (startX >= endX || startY >= endY) {
            return pixels;
        }
        
        int actualWidth = endX - startX;
        int actualHeight = endY - startY;
        pixels.reserve(actualWidth * actualHeight);
        
        for (int row = startY; row < endY; ++row) {
            for (int col = startX; col < endX; ++col) {
                pixels.push_back(m_pixelData[row * m_width + col]);
            }
        }
        
        return pixels;
    }
    
    void BitmapData::setPixels(int x, int y, int width, int height, const std::vector<uint32_t>& pixels)
    {
        if (!m_pixelData || pixels.empty()) {
            return;
        }
        
        // 确保区域在边界内
        int startX = std::max(0, x);
        int startY = std::max(0, y);
        int endX = std::min(m_width, x + width);
        int endY = std::min(m_height, y + height);
        
        if (startX >= endX || startY >= endY) {
            return;
        }
        
        int actualWidth = endX - startX;
        int actualHeight = endY - startY;
        
        size_t pixelIndex = 0;
        for (int row = startY; row < endY && pixelIndex < pixels.size(); ++row) {
            for (int col = startX; col < endX && pixelIndex < pixels.size(); ++col) {
                m_pixelData[row * m_width + col] = pixels[pixelIndex++];
            }
        }
    }
    
    std::string BitmapData::toDataURL(const std::string& type, double encoderOptions) const
    {
        // 简化实现：返回占位符base64字符串
        // 在实际项目中，这里应该使用图像编码库（如libpng、libjpeg等）
        std::stringstream ss;
        ss << "data:" << type << ";base64,";
        
        // 这里应该实现实际的图像编码
        // 当前返回一个简单的占位符
        ss << "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNk+M9QDwADhgGAWjR9awAAAABJRU5ErkJggg==";
        
        return ss.str();
    }
    
    std::shared_ptr<BitmapData> BitmapData::crop(std::shared_ptr<Rectangle> rect) const
    {
        if (!rect || !m_pixelData) {
            return nullptr;
        }
        
        int cropX = static_cast<int>(rect->getX());
        int cropY = static_cast<int>(rect->getY());
        int cropWidth = static_cast<int>(rect->getWidth());
        int cropHeight = static_cast<int>(rect->getHeight());
        
        // 确保裁剪区域在边界内
        cropX = std::max(0, std::min(cropX, m_width));
        cropY = std::max(0, std::min(cropY, m_height));
        cropWidth = std::min(cropWidth, m_width - cropX);
        cropHeight = std::min(cropHeight, m_height - cropY);
        
        if (cropWidth <= 0 || cropHeight <= 0) {
            return nullptr;
        }
        
        auto croppedData = create(cropWidth, cropHeight, true, 0);
        
        // 复制像素数据
        for (int y = 0; y < cropHeight; ++y) {
            for (int x = 0; x < cropWidth; ++x) {
                uint32_t pixel = getPixel32(cropX + x, cropY + y);
                croppedData->setPixel32(x, y, pixel);
            }
        }
        
        return croppedData;
    }
    
    void BitmapData::copyPixels(std::shared_ptr<BitmapData> sourceBitmapData,
                               std::shared_ptr<Rectangle> sourceRect,
                               std::shared_ptr<Rectangle> destPoint) const
    {
        if (!sourceBitmapData || !sourceRect || !destPoint) {
            return;
        }
        
        int srcX = static_cast<int>(sourceRect->getX());
        int srcY = static_cast<int>(sourceRect->getY());
        int srcWidth = static_cast<int>(sourceRect->getWidth());
        int srcHeight = static_cast<int>(sourceRect->getHeight());
        int destX = static_cast<int>(destPoint->getX());
        int destY = static_cast<int>(destPoint->getY());
        
        // 复制像素
        for (int y = 0; y < srcHeight; ++y) {
            for (int x = 0; x < srcWidth; ++x) {
                if (sourceBitmapData->isValidCoordinate(srcX + x, srcY + y) &&
                    isValidCoordinate(destX + x, destY + y)) {
                    uint32_t pixel = sourceBitmapData->getPixel32(srcX + x, srcY + y);
                    const_cast<BitmapData*>(this)->setPixel32(destX + x, destY + y, pixel);
                }
            }
        }
    }
    
    void BitmapData::fillRect(std::shared_ptr<Rectangle> rect, uint32_t color)
    {
        if (!rect || !m_pixelData) {
            return;
        }
        
        int fillX = static_cast<int>(rect->getX());
        int fillY = static_cast<int>(rect->getY());
        int fillWidth = static_cast<int>(rect->getWidth());
        int fillHeight = static_cast<int>(rect->getHeight());
        
        // 确保填充区域在边界内
        int startX = std::max(0, fillX);
        int startY = std::max(0, fillY);
        int endX = std::min(m_width, fillX + fillWidth);
        int endY = std::min(m_height, fillY + fillHeight);
        
        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                setPixel32(x, y, color);
            }
        }
    }
    
    void BitmapData::dispose()
    {
        if (m_disposed) {
            return;
        }
        
        // 从管理器中移除
        auto it = s_bitmapDataDisplayObjects.find(shared_from_this());
        if (it != s_bitmapDataDisplayObjects.end()) {
            s_bitmapDataDisplayObjects.erase(it);
        }
        
        // 释放像素数据
        deallocatePixelData();
        
        m_width = 0;
        m_height = 0;
        m_disposed = true;
    }
    
    // ========== 静态工厂方法实现 ==========
    
    std::shared_ptr<BitmapData> BitmapData::create(int width, int height, bool transparent, uint32_t fillColor)
    {
        if (width <= 0 || height <= 0) {
            return nullptr;
        }
        
        auto bitmapData = std::shared_ptr<BitmapData>(new BitmapData());
        bitmapData->initialize(width, height, transparent, fillColor);
        return bitmapData;
    }
    
    std::shared_ptr<BitmapData> BitmapData::createFromImageData(const std::vector<uint8_t>& imageData, int width, int height)
    {
        if (imageData.empty() || width <= 0 || height <= 0) {
            return nullptr;
        }
        
        auto bitmapData = std::shared_ptr<BitmapData>(new BitmapData());
        bitmapData->m_width = width;
        bitmapData->m_height = height;
        bitmapData->allocatePixelData();
        
        // 将字节数据转换为32位像素数据
        size_t expectedSize = width * height * 4; // RGBA
        if (imageData.size() >= expectedSize) {
            const uint8_t* srcData = imageData.data();
            uint32_t* destData = bitmapData->m_pixelData.get();
            
            for (int i = 0; i < width * height; ++i) {
                uint8_t r = srcData[i * 4 + 0];
                uint8_t g = srcData[i * 4 + 1];
                uint8_t b = srcData[i * 4 + 2];
                uint8_t a = srcData[i * 4 + 3];
                destData[i] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
        
        return bitmapData;
    }
    
    // ========== 静态管理方法实现 ==========
    
    void BitmapData::addDisplayObject(std::shared_ptr<DisplayObject> displayObject, std::shared_ptr<BitmapData> bitmapData)
    {
        if (!displayObject || !bitmapData) {
            return;
        }
        
        s_bitmapDataDisplayObjects[bitmapData].emplace(std::weak_ptr<DisplayObject>(displayObject));
    }
    
    void BitmapData::removeDisplayObject(std::shared_ptr<DisplayObject> displayObject, std::shared_ptr<BitmapData> bitmapData)
    {
        if (!displayObject || !bitmapData) {
            return;
        }
        
        auto it = s_bitmapDataDisplayObjects.find(bitmapData);
        if (it != s_bitmapDataDisplayObjects.end()) {
            // 查找并移除对应的weak_ptr
            auto& weakPtrSet = it->second;
            for (auto weakIt = weakPtrSet.begin(); weakIt != weakPtrSet.end();) {
                if (auto sharedPtr = weakIt->lock()) {
                    if (sharedPtr == displayObject) {
                        weakIt = weakPtrSet.erase(weakIt);
                        break;
                    } else {
                        ++weakIt;
                    }
                } else {
                    // 清理已过期的weak_ptr
                    weakIt = weakPtrSet.erase(weakIt);
                }
            }
            
            if (it->second.empty()) {
                s_bitmapDataDisplayObjects.erase(it);
            }
        }
    }
    
    void BitmapData::invalidate(std::shared_ptr<BitmapData> bitmapData)
    {
        if (!bitmapData) {
            return;
        }
        
        auto it = s_bitmapDataDisplayObjects.find(bitmapData);
        if (it != s_bitmapDataDisplayObjects.end()) {
            // 通知所有使用此BitmapData的显示对象进行更新
            for (auto& weakDisplayObject : it->second) {
                if (auto obj = weakDisplayObject.lock()) {
                    // obj->markRenderDirty();
                }
            }
        }
    }
    
    // ========== 受保护的辅助方法实现 ==========
    
    bool BitmapData::isValidCoordinate(int x, int y) const
    {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }
    
    // ========== 私有辅助方法实现 ==========
    
    void BitmapData::allocatePixelData()
    {
        if (m_width > 0 && m_height > 0) {
            m_pixelData = std::make_unique<uint32_t[]>(m_width * m_height);
        }
    }
    
    void BitmapData::deallocatePixelData()
    {
        m_pixelData.reset();
    }
    
    void BitmapData::initialize(int width, int height, bool transparent, uint32_t fillColor)
    {
        m_width = width;
        m_height = height;
        m_disposed = false;
        
        allocatePixelData();
        
        if (m_pixelData) {
            // 如果不支持透明度，强制Alpha为255
            if (!transparent) {
                fillColor |= 0xFF000000;
            }
            
            // 填充颜色
            std::fill_n(m_pixelData.get(), width * height, fillColor);
        }
    }
    
} // namespace egret