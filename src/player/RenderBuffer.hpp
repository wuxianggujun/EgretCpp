#pragma once
#include "core/HashObject.hpp"
#include <memory>

namespace egret {
namespace sys {

    /**
     * 渲染缓冲接口
     * 基于TypeScript egret.sys.RenderBuffer接口实现
     */
    class RenderBuffer : public HashObject {
    public:
        RenderBuffer();
        virtual ~RenderBuffer() = default;
        
        /**
         * 获取渲染表面（暂时使用void*，后续集成Skia时替换为SkCanvas*）
         */
        virtual void* getSurface() const { return m_surface; }
        
        /**
         * 获取渲染上下文（暂时使用void*，后续集成Skia时具体化）
         */
        virtual void* getContext() const { return m_context; }
        
        /**
         * 渲染缓冲的宽度，以像素为单位
         */
        virtual double getWidth() const { return m_width; }
        
        /**
         * 渲染缓冲的高度，以像素为单位
         */
        virtual double getHeight() const { return m_height; }
        
        /**
         * 改变渲染缓冲的大小并清空缓冲区
         * @param width 改变后的宽
         * @param height 改变后的高
         * @param useMaxSize 若传入true，则将改变后的尺寸与已有尺寸对比，保留较大的尺寸
         */
        virtual void resize(double width, double height, bool useMaxSize = false);
        
        /**
         * 清空缓冲区内容
         */
        virtual void clear();
        
        /**
         * 获取像素数据（碰撞检测用）
         * @param x x坐标
         * @param y y坐标
         * @param width 区域宽度
         * @param height 区域高度
         * @return 像素数据指针
         */
        virtual void* getPixels(double x, double y, double width, double height);
        
        /**
         * 销毁渲染缓冲
         */
        virtual void destroy();
        
    protected:
        void* m_surface = nullptr;      // 渲染表面
        void* m_context = nullptr;      // 渲染上下文
        double m_width = 0.0;          // 缓冲区宽度
        double m_height = 0.0;         // 缓冲区高度
    };

} // namespace sys
} // namespace egret