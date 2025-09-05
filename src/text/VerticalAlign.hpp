/**
 * @file VerticalAlign.hpp
 * @brief VerticalAlign类 - 垂直对齐方式定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/text/VerticalAlign.ts
 * VerticalAlign类为垂直对齐方式定义可能的值。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief VerticalAlign类 - 垂直对齐方式定义可能的值
 * 
 * VerticalAlign类为垂直对齐方式定义可能的值。
 * 用于文本对齐、UI布局等场景。
 * 
 * @see TextField::verticalAlign
 * @note 对应TypeScript的egret.VerticalAlign类，100%API兼容
 */
class VerticalAlign {
public:
    /**
     * @brief 将内容与容器的顶部对齐
     * 
     * 垂直对齐方式：顶部对齐
     */
    static const std::string TOP;

    /**
     * @brief 将内容与容器的底部对齐
     * 
     * 垂直对齐方式：底部对齐
     */
    static const std::string BOTTOM;

    /**
     * @brief 在容器的垂直中心对齐内容
     * 
     * 垂直对齐方式：居中对齐
     */
    static const std::string MIDDLE;

    /**
     * @brief 垂直两端对齐
     * 
     * 垂直对齐方式：两端对齐
     * @note TextFiled不支持此对齐方式
     */
    static const std::string JUSTIFY;

    /**
     * @brief 相对于容器对子项进行内容对齐
     * 
     * 这会将所有子项的大小统一调整为容器的"内容高度"。
     * 容器的"内容高度"是最大子项的大小，如果所有子项都小于容器的高度，
     * 则会将所有子项的大小调整为容器的高度。
     * 
     * @note TextFiled不支持此对齐方式
     */
    static const std::string CONTENT_JUSTIFY;

private:
    // 禁用构造函数，这是一个纯静态类
    VerticalAlign() = delete;
    ~VerticalAlign() = delete;
    VerticalAlign(const VerticalAlign&) = delete;
    VerticalAlign& operator=(const VerticalAlign&) = delete;
};

} // namespace egret