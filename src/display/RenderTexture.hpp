/**
 * @file RenderTexture.hpp
 * @brief RenderTexture类 - 动态纹理类，实现显示对象及子对象绘制成纹理的功能
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/RenderTexture.ts
 * RenderTexture是动态纹理类，可将显示对象及其子对象绘制成为一个纹理，支持离屏渲染。
 */

#pragma once

#include "display/Texture.hpp"
#include <memory>
#include <vector>

namespace egret {

// 前置声明
class DisplayObject;
class Rectangle;
namespace sys {
    class RenderBuffer;
}

/**
 * @brief RenderTexture类 - 动态纹理类，支持离屏渲染
 * 
 * RenderTexture是一个动态纹理，可以将显示对象及其子对象绘制成为一个纹理。
 * 这对于创建动态内容、缓存复杂的显示对象、实现特效等功能非常有用。
 * 在WebGL/OpenGL上，这种做法可以显著提升性能。
 * 
 * @note 对应TypeScript的egret.RenderTexture类，100%API兼容
 * @extends Texture
 */
class RenderTexture : public Texture {
public:
    /**
     * @brief 构造函数 - 创建新的RenderTexture实例
     * 
     * 创建一个RenderTexture对象，自动初始化内部的RenderBuffer。
     */
    RenderTexture();

    /**
     * @brief 析构函数
     * 
     * 清理RenderBuffer和相关资源。
     */
    virtual ~RenderTexture();

    /**
     * @brief 将指定显示对象绘制为一个纹理
     * 
     * 这是RenderTexture的核心功能。将指定的显示对象及其所有子对象
     * 渲染到这个RenderTexture中。
     * 
     * @param displayObject 需要绘制的显示对象
     * @param clipBounds 绘制矩形区域，为空则使用对象的完整边界
     * @param scale 缩放比例，默认为1.0
     * @return true 绘制成功，false 绘制失败（通常是因为边界为空）
     * 
     * @note 对应TypeScript的drawToTexture()方法
     */
    bool drawToTexture(DisplayObject* displayObject, Rectangle* clipBounds = nullptr, double scale = 1.0);

    /**
     * @brief 获取指定位置的像素颜色值（RGBA数组）
     * 
     * 返回RenderTexture中指定像素位置的颜色值。
     * 
     * @param x 像素X坐标
     * @param y 像素Y坐标
     * @return 像素颜色值数组 [R, G, B, A]，范围0-255
     * 
     * @note 对应TypeScript的getPixel32()方法
     */
    std::vector<int> getPixel32(int x, int y) const;

    /**
     * @brief 释放纹理资源
     * 
     * 重写父类方法，额外释放RenderBuffer资源。
     * 
     * @note 对应TypeScript的dispose()方法
     */
    void dispose();

    /**
     * @brief 获取内部RenderBuffer（调试和高级用途）
     * 
     * 获取RenderTexture内部使用的RenderBuffer对象。
     * 通常用于调试或高级渲染操作。
     * 
     * @return RenderBuffer指针，可能为nullptr
     */
    sys::RenderBuffer* getRenderBuffer() const;

protected:
    /**
     * @brief 内部渲染缓冲区
     * 
     * RenderTexture内部使用的渲染缓冲区，用于实际的离屏渲染。
     * 对应TypeScript中的$renderBuffer成员。
     */
    std::shared_ptr<sys::RenderBuffer> m_renderBuffer;

private:
    // 禁用拷贝构造和赋值操作
    RenderTexture(const RenderTexture&) = delete;
    RenderTexture& operator=(const RenderTexture&) = delete;
};

} // namespace egret