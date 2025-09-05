#pragma once
#include "player/RenderBuffer.hpp"
#include <memory>
#include <string>
#include <vector>

// Skia头文件包含
#include <include/core/SkRefCnt.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkImageInfo.h>

namespace egret {
namespace sys {

    /**
     * SkiaRenderBuffer - 基于Skia的渲染缓冲区实现
     * 提供真正的Skia Surface和Canvas集成
     */
    class SkiaRenderBuffer : public RenderBuffer {
    public:
        SkiaRenderBuffer();
        virtual ~SkiaRenderBuffer();
        
        // ========== RenderBuffer接口重写 ==========
        
        /**
         * 获取Skia画布
         */
        void* getSurface() const override;
        
        /**
         * 获取SkSurface指针
         */
        void* getContext() const override;
        
        /**
         * 改变渲染缓冲的大小并重新创建Skia Surface
         */
        void resize(double width, double height, bool useMaxSize = false) override;
        
        /**
         * 清空Skia画布
         */
        void clear() override;
        
        /**
         * 获取像素数据（用于碰撞检测）
         */
        void* getPixels(double x, double y, double width, double height) override;
        
        /**
         * 销毁Skia渲染资源
         */
        void destroy() override;
        
        // ========== Skia特有方法 ==========
        
        /**
         * 获取SkCanvas指针（类型安全）
         */
        SkCanvas* getSkCanvas() const { return m_canvas; }
        
        /**
         * 获取SkSurface指针（类型安全）
         */
        SkSurface* getSkSurface() const { return m_skSurface.get(); }
        
        /**
         * 保存画布内容到PNG文件（调试用）
         */
        bool saveToPNG(const std::string& filename);
        
        /**
         * 从现有SkSurface创建RenderBuffer
         */
        static std::shared_ptr<SkiaRenderBuffer> createFromSurface(sk_sp<SkSurface> surface);
        
        /**
         * 检查是否已初始化
         */
        bool isValid() const { return m_skSurface && m_canvas; }
        
    private:
        /**
         * 创建Skia Surface
         */
        void createSkiaSurface(int width, int height);
        
        /**
         * 释放Skia资源
         */
        void releaseSkiaResources();
        
        // ========== 私有成员变量 ==========
        
        sk_sp<SkSurface> m_skSurface;              // Skia表面
        SkCanvas* m_canvas = nullptr;               // Skia画布（由Surface拥有）
        
        // 像素数据缓存（用于碰撞检测）
        mutable std::vector<uint32_t> m_pixelCache;
    };
    
    // ========== 工厂方法 ==========
    
    /**
     * 创建Skia渲染缓冲区
     */
    std::shared_ptr<SkiaRenderBuffer> createSkiaRenderBuffer(double width = 0, double height = 0);

} // namespace sys
} // namespace egret