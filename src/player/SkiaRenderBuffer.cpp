#include "player/SkiaRenderBuffer.hpp"

// Skia头文件
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkImageInfo.h>
#include <include/core/SkPixmap.h>
#include <include/core/SkImage.h>
#include <include/core/SkData.h>
#include <include/encode/SkPngEncoder.h>

#include <algorithm>
#include <fstream>

namespace egret {
namespace sys {

    SkiaRenderBuffer::SkiaRenderBuffer() : RenderBuffer() {
        // 初始构造，Surface将在第一次resize时创建
    }
    
    SkiaRenderBuffer::~SkiaRenderBuffer() {
        releaseSkiaResources();
    }
    
    // ========== RenderBuffer接口实现 ==========
    
    void* SkiaRenderBuffer::getSurface() const {
        return m_canvas;  // 返回SkCanvas指针供SkiaRenderer使用
    }
    
    void* SkiaRenderBuffer::getContext() const {
        return m_skSurface.get();  // 返回SkSurface指针
    }
    
    void SkiaRenderBuffer::resize(double width, double height, bool useMaxSize) {
        if (useMaxSize) {
            width = std::max(width, m_width);
            height = std::max(height, m_height);
        }
        
        int intWidth = static_cast<int>(std::ceil(width));
        int intHeight = static_cast<int>(std::ceil(height));
        
        // 如果尺寸未变化且已有有效Surface，无需重新创建
        if (m_width == width && m_height == height && isValid()) {
            return;
        }
        
        m_width = width;
        m_height = height;
        
        // 重新创建Skia Surface
        if (intWidth > 0 && intHeight > 0) {
            createSkiaSurface(intWidth, intHeight);
        } else {
            releaseSkiaResources();
        }
    }
    
    void SkiaRenderBuffer::clear() {
        if (m_canvas) {
            // 使用透明色清空画布
            m_canvas->clear(SK_ColorTRANSPARENT);
        }
    }
    
    void* SkiaRenderBuffer::getPixels(double x, double y, double width, double height) {
        if (!isValid()) {
            return nullptr;
        }
        
        // 计算像素区域
        int left = static_cast<int>(std::floor(x));
        int top = static_cast<int>(std::floor(y));
        int right = static_cast<int>(std::ceil(x + width));
        int bottom = static_cast<int>(std::ceil(y + height));
        
        // 边界检查
        left = std::max(0, left);
        top = std::max(0, top);
        right = std::min(static_cast<int>(m_width), right);
        bottom = std::min(static_cast<int>(m_height), bottom);
        
        if (left >= right || top >= bottom) {
            return nullptr;
        }
        
        int pixelWidth = right - left;
        int pixelHeight = bottom - top;
        
        // 调整像素缓存大小
        m_pixelCache.resize(pixelWidth * pixelHeight);
        
        // 读取像素数据
        SkImageInfo info = SkImageInfo::MakeN32Premul(pixelWidth, pixelHeight);
        if (m_skSurface->readPixels(info, m_pixelCache.data(), pixelWidth * sizeof(uint32_t), left, top)) {
            return m_pixelCache.data();
        }
        
        return nullptr;
    }
    
    void SkiaRenderBuffer::destroy() {
        releaseSkiaResources();
        m_width = 0.0;
        m_height = 0.0;
        m_pixelCache.clear();
        
        // 调用基类的destroy
        RenderBuffer::destroy();
    }
    
    // ========== Skia特有方法实现 ==========
    
    bool SkiaRenderBuffer::saveToPNG(const std::string& filename) {
        if (!isValid()) {
            return false;
        }
        
        // 获取Surface的图像
        sk_sp<SkImage> image = m_skSurface->makeImageSnapshot();
        if (!image) {
            return false;
        }
        
        // 编码为PNG
        sk_sp<SkData> pngData = SkPngEncoder::Encode(nullptr, image.get(), {});
        if (!pngData) {
            return false;
        }
        
        // 写入文件
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file.write(static_cast<const char*>(pngData->data()), pngData->size());
        return file.good();
    }
    
    std::shared_ptr<SkiaRenderBuffer> SkiaRenderBuffer::createFromSurface(sk_sp<SkSurface> surface) {
        if (!surface) {
            return nullptr;
        }
        
        auto buffer = std::make_shared<SkiaRenderBuffer>();
        buffer->m_skSurface = surface;
        buffer->m_canvas = surface->getCanvas();
        
        // 获取尺寸信息
        SkImageInfo info = surface->imageInfo();
        buffer->m_width = static_cast<double>(info.width());
        buffer->m_height = static_cast<double>(info.height());
        
        return buffer;
    }
    
    // ========== 私有方法实现 ==========
    
    void SkiaRenderBuffer::createSkiaSurface(int width, int height) {
        // 释放旧资源
        releaseSkiaResources();
        
        // 创建图像信息
        SkImageInfo info = SkImageInfo::MakeN32Premul(width, height);
        
        // 创建栅格表面（CPU渲染）
        m_skSurface = SkSurfaces::Raster(info);
        
        if (m_skSurface) {
            m_canvas = m_skSurface->getCanvas();
            
            // 初始化画布（清空为透明）
            clear();
        }
    }
    
    void SkiaRenderBuffer::releaseSkiaResources() {
        m_canvas = nullptr;  // Canvas由Surface拥有，不需要手动删除
        m_skSurface.reset(); // 智能指针自动释放
    }
    
    // ========== 工厂方法实现 ==========
    
    std::shared_ptr<SkiaRenderBuffer> createSkiaRenderBuffer(double width, double height) {
        auto buffer = std::make_shared<SkiaRenderBuffer>();
        
        if (width > 0 && height > 0) {
            buffer->resize(width, height);
        }
        
        return buffer;
    }

} // namespace sys
} // namespace egret