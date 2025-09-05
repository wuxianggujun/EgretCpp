/**
 * @file HorizontalAlign.hpp
 * @brief HorizontalAlign类 - 水平对齐方式定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/text/HorizontalAlign.ts
 * HorizontalAlign类为水平对齐方式定义可能的值。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief HorizontalAlign类 - 水平对齐方式定义可能的值
 * 
 * HorizontalAlign类为水平对齐方式定义可能的值。
 * 用于文本对齐、UI布局等场景。
 * 
 * @see TextField::textAlign
 * @note 对应TypeScript的egret.HorizontalAlign类，100%API兼容
 */
class HorizontalAlign {
public:
    /**
     * @brief 将内容与容器的左侧对齐
     * 
     * 水平对齐方式：左对齐
     */
    static const std::string LEFT;

    /**
     * @brief 将内容与容器的右侧对齐
     * 
     * 水平对齐方式：右对齐
     */
    static const std::string RIGHT;

    /**
     * @brief 在容器的水平中心对齐内容
     * 
     * 水平对齐方式：居中对齐
     */
    static const std::string CENTER;

    /**
     * @brief 水平两端对齐
     * 
     * 水平对齐方式：两端对齐
     * @note TextFiled不支持此对齐方式
     */
    static const std::string JUSTIFY;

    /**
     * @brief 相对于容器对子项进行内容对齐
     * 
     * 这会将所有子项的大小统一调整为容器的"内容宽度"。
     * 容器的"内容宽度"是最大子项的大小，如果所有子项都小于容器的宽度，
     * 则会将所有子项的大小调整为容器的宽度。
     * 
     * @note TextFiled不支持此对齐方式
     */
    static const std::string CONTENT_JUSTIFY;

private:
    // 禁用构造函数，这是一个纯静态类
    HorizontalAlign() = delete;
    ~HorizontalAlign() = delete;
    HorizontalAlign(const HorizontalAlign&) = delete;
    HorizontalAlign& operator=(const HorizontalAlign&) = delete;
};

} // namespace egret