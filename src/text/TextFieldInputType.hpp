/**
 * @file TextFieldInputType.hpp
 * @brief TextFieldInputType类 - 文本字段输入类型定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/text/TextFieldInputType.ts
 * TextFieldInputType类是在设置TextField类的inputType属性时使用的常数值的枚举。
 */

#pragma once

#include <string>

namespace egret {

/**
 * @brief TextFieldInputType类 - 文本字段输入类型定义
 * 
 * TextFieldInputType类是在设置TextField类的inputType属性时使用的常数值的枚举。
 * 定义了不同的输入文本类型，用于优化移动设备的虚拟键盘显示。
 * 
 * @see TextField::inputType
 * @note 对应TypeScript的egret.TextFieldInputType类，100%API兼容
 */
class TextFieldInputType {
public:
    /**
     * @brief 默认 input 类型
     * 
     * 默认文本输入类型：标准文本输入
     */
    static const std::string TEXT;

    /**
     * @brief 电话号码 input 类型
     * 
     * 电话号码输入类型：专门用于输入电话号码，
     * 移动设备会显示数字键盘
     */
    static const std::string TEL;

    /**
     * @brief password 类型
     * 
     * 密码输入类型：输入内容会被隐藏显示
     */
    static const std::string PASSWORD;

private:
    // 禁用构造函数，这是一个纯静态类
    TextFieldInputType() = delete;
    ~TextFieldInputType() = delete;
    TextFieldInputType(const TextFieldInputType&) = delete;
    TextFieldInputType& operator=(const TextFieldInputType&) = delete;
};

} // namespace egret