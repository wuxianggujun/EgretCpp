#pragma once

#include <functional>
#include <vector>
#include <any>

namespace egret {

    /**
     * 延迟调用系统
     * 对应 TypeScript 的 callLater 相关功能
     */
    namespace CallLaterSystem {
        
        /**
         * 延迟函数参数类型
         */
        using CallLaterFunction = std::function<void()>;
        
        /**
         * 延迟函数到屏幕重绘前执行
         * @param func 要延迟执行的函数
         */
        void callLater(CallLaterFunction func);
        
        /**
         * 异步调用函数  
         * @param func 要异步调用的函数
         */
        void callAsync(CallLaterFunction func);
        
        /**
         * 执行所有延迟调用的函数
         * 由SystemTicker在渲染时调用
         */
        void executeLaters();
        
        /**
         * 执行所有异步调用的函数
         * 由SystemTicker在每帧开始时调用
         */
        void executeAsyncs();
        
        /**
         * 清空所有待执行的函数
         */
        void clear();
    }

} // namespace egret