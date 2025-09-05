/**
 * @file BitmapFillMode.cpp
 * @brief BitmapFillMode枚举实现 - 位图填充模式定义
 * 
 * 翻译自：egret-core-5.4.1/src/egret/display/BitmapFillMode.ts
 * BitmapFillMode定义了位图在容器中的填充方式。
 */

#include "display/BitmapFillMode.hpp"

namespace egret {

// ========== BitmapFillMode静态常量定义 ==========

const std::string BitmapFillMode::SCALE = "scale";
const std::string BitmapFillMode::CLIP = "clip";  
const std::string BitmapFillMode::REPEAT = "repeat";

} // namespace egret