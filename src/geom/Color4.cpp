/**
 * @file Color4.cpp
 * @brief Color4类实现 - 四分量颜色(RGBA)
 */

#include "geom/Color4.hpp"

namespace egret {

// 预定义颜色常量定义
const Color4 Color4::BLACK(0.0, 0.0, 0.0, 1.0);
const Color4 Color4::WHITE(1.0, 1.0, 1.0, 1.0);
const Color4 Color4::RED(1.0, 0.0, 0.0, 1.0);
const Color4 Color4::GREEN(0.0, 1.0, 0.0, 1.0);
const Color4 Color4::BLUE(0.0, 0.0, 1.0, 1.0);
const Color4 Color4::TRANSPARENT(0.0, 0.0, 0.0, 0.0);

} // namespace egret