/**
 * @file LineStyles.cpp
 * @brief 线条样式枚举实现 - CapsStyle、JointStyle等线条绘制相关的常量值
 * 
 * 翻译自：
 * - egret-core-5.4.1/src/egret/display/CapsStyle.ts
 * - egret-core-5.4.1/src/egret/display/JointStyle.ts
 * 
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "display/LineStyles.hpp"

namespace egret {

    // ========== CapsStyle常量定义 ==========

    const std::string CapsStyle::NONE = "none";
    const std::string CapsStyle::ROUND = "round";
    const std::string CapsStyle::SQUARE = "square";

    // ========== JointStyle常量定义 ==========

    const std::string JointStyle::BEVEL = "bevel";
    const std::string JointStyle::MITER = "miter";
    const std::string JointStyle::ROUND = "round";

} // namespace egret