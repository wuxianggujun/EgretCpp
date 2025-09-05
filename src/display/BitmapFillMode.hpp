/**
 * @file BitmapFillMode.hpp
 * @brief BitmapFillMode枚举 - 位图填充模式定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/BitmapFillMode.ts
 * BitmapFillMode定义了位图在容器中的填充方式。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief BitmapFillMode类 - 位图填充模式常量定义
 * 
 * BitmapFillMode类定义位图在容器中如何填充的模式常量。
 * 完全基于TypeScript egret.BitmapFillMode实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.BitmapFillMode类
 */
class BitmapFillMode {
public:
    /**
     * @brief 缩放模式 - 对应 public static SCALE:string = "scale";
     * 
     * 位图会拉伸以填充整个容器
     */
    static const std::string SCALE;
    
    /**
     * @brief 裁剪模式 - 对应 public static CLIP:string = "clip";
     * 
     * 位图按原尺寸显示，超出容器部分被裁剪
     */
    static const std::string CLIP;
    
    /**
     * @brief 重复模式 - 对应 public static REPEAT:string = "repeat";
     * 
     * 位图按原尺寸重复填充整个容器
     */
    static const std::string REPEAT;

private:
    // 禁止实例化，这是一个静态常量类
    BitmapFillMode() = delete;
};

} // namespace egret