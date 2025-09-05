/**
 * @file BlendMode.hpp
 * @brief BlendMode枚举类 - 提供混合模式可视效果的常量值
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/BlendMode.ts
 * BlendMode提供混合模式可视效果的常量值，通常用于DisplayObject的blendMode属性上。
 * 混合模式在Web端只有部分被支持，在Native中全部都支持。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief BlendMode类 - 提供混合模式可视效果的常量值
 * 
 * 提供混合模式可视效果的常量值的类，通常用于DisplayObject的blendMode属性上。
 * 
 * 支持情况说明：
 * - 目前所有平台的浏览器都支持的有：Layer、Alpha、Normal、Add、ERASE
 * - iOS中的所有浏览器以及Android内的部分浏览器还支持：Multiply、Screen、Lighten、Darken、Difference、Overlay、HardLight
 * - 仅在Native端支持的有：Subtract、Invert
 * 
 * @note 对应TypeScript的egret.BlendMode类，100%API兼容
 * @see DisplayObject#setBlendMode()
 */
class BlendMode {
public:
    /**
     * @brief 正常模式 - 该显示对象出现在背景前面
     * 
     * 显示对象的像素值会覆盖背景的像素值。
     * 在显示对象为透明的区域，背景是可见的。
     * 
     * @note 对应TypeScript的BlendMode.NORMAL
     */
    static const std::string NORMAL;

    /**
     * @brief 加法模式 - 将显示对象的原色值添加到它的背景颜色中
     * 
     * 将显示对象的原色值添加到它的背景颜色中，上限值为0xFF。
     * 此设置通常用于使两个对象间的加亮溶解产生动画效果。
     * 
     * 例如：如果显示对象的某个像素的RGB值为0xAAA633，背景像素的RGB值为0xDD2200，
     * 则显示像素的结果RGB值为0xFFC833（因为0xAA + 0xDD > 0xFF，0xA6 + 0x22 = 0xC8，且0x33 + 0x00 = 0x33）。
     * 
     * @note 对应TypeScript的BlendMode.ADD
     */
    static const std::string ADD;

    /**
     * @brief 擦除模式 - 根据显示对象的Alpha值擦除背景
     * 
     * 根据显示对象的Alpha值擦除背景。Alpha值不为0的区域将被擦除。
     * 
     * @note 对应TypeScript的BlendMode.ERASE
     */
    static const std::string ERASE;

    // 扩展混合模式（部分浏览器支持）

    /**
     * @brief 乘法模式 - 将显示对象的颜色与背景颜色相乘
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string MULTIPLY;

    /**
     * @brief 屏幕模式 - 与乘法模式相反的效果
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string SCREEN;

    /**
     * @brief 变亮模式 - 选择较亮的颜色
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string LIGHTEN;

    /**
     * @brief 变暗模式 - 选择较暗的颜色
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string DARKEN;

    /**
     * @brief 差值模式 - 从较亮的颜色中减去较暗的颜色
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string DIFFERENCE;

    /**
     * @brief 叠加模式 - 根据背景颜色应用乘法或屏幕模式
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string OVERLAY;

    /**
     * @brief 强光模式 - 根据显示对象颜色应用乘法或屏幕模式
     * 
     * @note 仅在部分浏览器和Native端支持
     */
    static const std::string HARD_LIGHT;

    // Native专有混合模式

    /**
     * @brief 减法模式 - 从背景颜色中减去显示对象颜色
     * 
     * @note 仅在Native端支持
     */
    static const std::string SUBTRACT;

    /**
     * @brief 反转模式 - 反转背景颜色
     * 
     * @note 仅在Native端支持
     */
    static const std::string INVERT;

private:
    // 禁止实例化
    BlendMode() = delete;
    ~BlendMode() = delete;
    BlendMode(const BlendMode&) = delete;
    BlendMode& operator=(const BlendMode&) = delete;
};

namespace sys {

/**
 * @brief 转换blendMode字符串为数字
 * 
 * 将混合模式字符串转换为对应的数字值，用于内部渲染系统。
 * 
 * @param blendMode 混合模式字符串
 * @return 对应的数字值，未知混合模式返回0（NORMAL）
 * 
 * @note 对应TypeScript的sys.blendModeToNumber()函数
 */
int blendModeToNumber(const std::string& blendMode);

/**
 * @brief 转换数字为blendMode字符串
 * 
 * 将数字值转换为对应的混合模式字符串。
 * 
 * @param blendMode 混合模式数字值
 * @return 对应的字符串，未知数字返回"normal"
 * 
 * @note 对应TypeScript的sys.numberToBlendMode()函数
 */
std::string numberToBlendMode(int blendMode);

} // namespace sys

} // namespace egret