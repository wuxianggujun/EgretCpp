/**
 * @file TextFieldType.hpp
 * @brief TextFieldType类 - 文本字段类型定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/text/TextFieldType.ts
 * TextFieldType类是在设置TextField类的type属性时使用的常数值的枚举。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief TextFieldType类 - 文本字段类型定义
 * 
 * TextFieldType类是在设置TextField类的type属性时使用的常数值的枚举。
 * 定义了文本字段的不同显示和交互模式。
 * 
 * @see TextField::type
 * @note 对应TypeScript的egret.TextFieldType类，100%API兼容
 */
class TextFieldType {
public:
    /**
     * @brief 用于指定动态文本
     * 
     * 动态文本模式：文本只用于显示，不可编辑
     */
    static const std::string DYNAMIC;

    /**
     * @brief 用于指定输入文本
     * 
     * 输入文本模式：文本可编辑，允许用户输入
     */
    static const std::string INPUT;

private:
    // 禁用构造函数，这是一个纯静态类
    TextFieldType() = delete;
    ~TextFieldType() = delete;
    TextFieldType(const TextFieldType&) = delete;
    TextFieldType& operator=(const TextFieldType&) = delete;
};

} // namespace egret