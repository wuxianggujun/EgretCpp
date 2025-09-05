#include "player/RenderBuffer.hpp"
#include <algorithm>

namespace egret {
namespace sys {

    RenderBuffer::RenderBuffer() : HashObject() {
        // 基础构造函数，具体实现待Skia集成时完善
    }
    
    void RenderBuffer::resize(double width, double height, bool useMaxSize) {
        if (useMaxSize) {
            width = std::max(width, m_width);
            height = std::max(height, m_height);
        }
        
        if (m_width == width && m_height == height) {
            return; // 尺寸未变化，无需重新分配
        }
        
        m_width = width;
        m_height = height;
        
        // TODO: 当集成Skia时，在这里重新创建渲染表面
        // 例如：m_surface = SkSurface::MakeRaster(SkImageInfo::MakeN32Premul(width, height));
    }
    
    void RenderBuffer::clear() {
        // TODO: 当集成Skia时，在这里清空画布
        // 例如：if (m_surface) static_cast<SkCanvas*>(m_context)->clear(SK_ColorTRANSPARENT);
    }
    
    void* RenderBuffer::getPixels(double x, double y, double width, double height) {
        // TODO: 当集成Skia时，在这里获取像素数据用于碰撞检测
        // 这个方法主要用于hitTest功能
        return nullptr;
    }
    
    void RenderBuffer::destroy() {
        // TODO: 当集成Skia时，在这里释放渲染资源
        m_surface = nullptr;
        m_context = nullptr;
        m_width = 0.0;
        m_height = 0.0;
    }

} // namespace sys
} // namespace egret