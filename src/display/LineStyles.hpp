/**
 * @file LineStyles.hpp
 * @brief 线条样式枚举 - CapsStyle、JointStyle等线条绘制相关的常量值
 * 
 * 翻译自：
 * - egret-core-5.4.1/src/egret/display/CapsStyle.ts
 * - egret-core-5.4.1/src/egret/display/JointStyle.ts
 * 
 * 提供绘制线条时使用的各种样式常量，用于Graphics.lineStyle()方法。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief CapsStyle类 - 线条端点样式枚举
 * 
 * 可指定在绘制线条中使用的端点样式的常量值枚举。
 * 常量可用作Graphics.lineStyle()方法的caps参数中的值。
 * 
 * @note 对应TypeScript的egret.CapsStyle，100%API兼容
 * @see Graphics#lineStyle()
 */
class CapsStyle {
public:
    /**
     * @brief 无端点样式
     * 
     * 用于在Graphics.lineStyle()方法的caps参数中指定没有端点。
     * 线条在端点处直接截断。
     * 
     * @note 对应TypeScript的CapsStyle.NONE
     */
    static const std::string NONE;

    /**
     * @brief 圆头端点样式
     * 
     * 用于在Graphics.lineStyle()方法的caps参数中指定圆头端点。
     * 在线条的每个端点添加一个半圆形的端点。
     * 
     * @note 对应TypeScript的CapsStyle.ROUND
     */
    static const std::string ROUND;

    /**
     * @brief 方头端点样式
     * 
     * 用于在Graphics.lineStyle()方法的caps参数中指定方头端点。
     * 在线条的每个端点添加一个矩形的端点。
     * 
     * @note 对应TypeScript的CapsStyle.SQUARE
     */
    static const std::string SQUARE;

private:
    // 禁止实例化
    CapsStyle() = delete;
    ~CapsStyle() = delete;
    CapsStyle(const CapsStyle&) = delete;
    CapsStyle& operator=(const CapsStyle&) = delete;
};

/**
 * @brief JointStyle类 - 线条连接点样式枚举
 * 
 * 指定要在绘制线条中使用的连接点样式的常量值枚举。
 * 提供的这些常量用作Graphics.lineStyle()方法的joints参数中的值。
 * 
 * @note 对应TypeScript的egret.JointStyle，100%API兼容
 * @see Graphics#lineStyle()
 */
class JointStyle {
public:
    /**
     * @brief 斜角连接样式
     * 
     * 在Graphics.lineStyle()方法的joints参数中指定斜角连接。
     * 在线条的连接处创建一个平直的斜切边。
     * 
     * @note 对应TypeScript的JointStyle.BEVEL
     */
    static const std::string BEVEL;

    /**
     * @brief 尖角连接样式
     * 
     * 在Graphics.lineStyle()方法的joints参数中指定尖角连接。
     * 在线条的连接处延伸外边缘直到相交，形成尖角。
     * 
     * @note 对应TypeScript的JointStyle.MITER
     */
    static const std::string MITER;

    /**
     * @brief 圆角连接样式
     * 
     * 在Graphics.lineStyle()方法的joints参数中指定圆角连接。
     * 在线条的连接处创建一个圆形的连接。
     * 
     * @note 对应TypeScript的JointStyle.ROUND
     */
    static const std::string ROUND;

private:
    // 禁止实例化
    JointStyle() = delete;
    ~JointStyle() = delete;
    JointStyle(const JointStyle&) = delete;
    JointStyle& operator=(const JointStyle&) = delete;
};

} // namespace egret