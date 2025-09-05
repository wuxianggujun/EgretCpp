/**
 * @file TextFormat.hpp
 * @brief TextFormat结构体 - 文本格式定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/TextFormat.ts
 * TextFormat结构体定义文本的格式属性。
 */

#pragma once

#include <string>
#include <optional>

namespace egret {
namespace sys {

/**
 * @brief TextFormat结构体 - 文本格式定义
 * 
 * TextFormat结构体定义文本的格式属性，包括颜色、字体、大小、样式等。
 * 使用std::optional使所有属性都可选，对应TypeScript中的?属性。
 * 
 * @note 对应TypeScript的egret.sys.TextFormat接口，100%API兼容
 */
struct TextFormat {
    /**
     * @brief 颜色值
     * 
     * 文本颜色，0xRRGGBB格式
     */
    std::optional<uint32_t> textColor;

    /**
     * @brief 描边颜色值
     * 
     * 文本描边颜色，0xRRGGBB格式
     */
    std::optional<uint32_t> strokeColor;

    /**
     * @brief 字号
     * 
     * 字体大小（像素）
     */
    std::optional<double> size;

    /**
     * @brief 描边大小
     * 
     * 文本描边宽度（像素）
     */
    std::optional<double> stroke;

    /**
     * @brief 是否加粗
     * 
     * true表示粗体，false表示正常
     */
    std::optional<bool> bold;

    /**
     * @brief 是否倾斜
     * 
     * true表示斜体，false表示正常
     */
    std::optional<bool> italic;

    /**
     * @brief 字体名称
     * 
     * 字体族名称（如"Arial", "Times New Roman"等）
     */
    std::optional<std::string> fontFamily;

    /**
     * @brief 默认构造函数
     * 
     * 创建空的文本格式，所有属性都未设置
     */
    TextFormat() = default;

    /**
     * @brief 拷贝构造函数
     */
    TextFormat(const TextFormat& other) = default;

    /**
     * @brief 赋值操作符
     */
    TextFormat& operator=(const TextFormat& other) = default;
};

} // namespace sys
} // namespace egret