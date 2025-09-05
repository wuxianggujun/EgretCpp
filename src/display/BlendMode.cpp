/**
 * @file BlendMode.cpp
 * @brief BlendMode枚举类实现 - 提供混合模式可视效果的常量值
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/BlendMode.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "display/BlendMode.hpp"
#include <unordered_map>
#include <vector>

namespace egret {

    // ========== BlendMode常量定义 ==========

    // 核心混合模式（所有平台支持）
    const std::string BlendMode::NORMAL = "normal";
    const std::string BlendMode::ADD = "add";
    const std::string BlendMode::ERASE = "erase";

    // 扩展混合模式（部分浏览器支持）
    const std::string BlendMode::MULTIPLY = "multiply";
    const std::string BlendMode::SCREEN = "screen";
    const std::string BlendMode::LIGHTEN = "lighten";
    const std::string BlendMode::DARKEN = "darken";
    const std::string BlendMode::DIFFERENCE = "difference";
    const std::string BlendMode::OVERLAY = "overlay";
    const std::string BlendMode::HARD_LIGHT = "hardLight";

    // Native专有混合模式
    const std::string BlendMode::SUBTRACT = "subtract";
    const std::string BlendMode::INVERT = "invert";

namespace sys {

    // ========== 内部转换系统 ==========

    /**
     * @brief 混合模式字符串数组
     * 
     * 对应TypeScript中的blendModeString数组。
     * 顺序很重要，索引对应数字值。
     */
    static const std::vector<std::string> blendModeString = {
        "normal",      // 0
        "add",         // 1
        "erase",       // 2
        "multiply",    // 3
        "screen",      // 4
        "lighten",     // 5
        "darken",      // 6
        "difference",  // 7
        "overlay",     // 8
        "hardLight",   // 9
        "subtract",    // 10
        "invert"       // 11
    };

    /**
     * @brief 混合模式字符串到数字的映射表
     * 
     * 对应TypeScript中的blendModeNumber对象。
     * 使用静态初始化确保映射表只创建一次。
     */
    static const std::unordered_map<std::string, int> blendModeNumber = []() {
        std::unordered_map<std::string, int> map;
        for (size_t i = 0; i < blendModeString.size(); ++i) {
            map[blendModeString[i]] = static_cast<int>(i);
        }
        return map;
    }();

    // ========== 公共转换函数 ==========

    int blendModeToNumber(const std::string& blendMode) {
        // 对应TypeScript中的let num = blendModeNumber[blendMode]; return num === undefined ? 0 : num;
        auto it = blendModeNumber.find(blendMode);
        return (it != blendModeNumber.end()) ? it->second : 0;
    }

    std::string numberToBlendMode(int blendMode) {
        // 对应TypeScript中的let str = blendModeString[blendMode]; return str === undefined ? "normal" : str;
        if (blendMode >= 0 && blendMode < static_cast<int>(blendModeString.size())) {
            return blendModeString[blendMode];
        }
        return "normal";
    }

} // namespace sys

} // namespace egret