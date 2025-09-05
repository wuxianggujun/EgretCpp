#pragma once

#include <chrono>

namespace egret {

    /**
     * 获取相对时间工具函数
     * 对应 TypeScript 的 egret.getTimer()
     * @return 自引擎启动以来的毫秒数
     */
    inline long long getTimer() {
        static auto startTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        return duration.count();
    }

    namespace sys {
        /**
         * 引擎启动时间
         * 对应 TypeScript 的 sys.$START_TIME
         */
        extern long long START_TIME;

        /**
         * 是否需要广播 RENDER 事件的标志
         * 对应 TypeScript 的 sys.$invalidateRenderFlag  
         */
        extern bool invalidateRenderFlag;

        /**
         * 需要立即刷新屏幕的标志
         * 对应 TypeScript 的 sys.$requestRenderingFlag
         */
        extern bool requestRenderingFlag;
    }

} // namespace egret