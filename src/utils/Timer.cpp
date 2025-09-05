#include "utils/Timer.hpp"
#include <chrono>

namespace egret {
namespace sys {

    // 全局变量定义
    long long START_TIME = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    bool invalidateRenderFlag = false;
    bool requestRenderingFlag = false;

} // namespace sys
} // namespace egret