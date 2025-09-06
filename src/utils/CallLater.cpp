#include "utils/CallLater.hpp"
#include <iostream>
#include "utils/Logger.hpp"

namespace egret {
namespace CallLaterSystem {

    // 延迟函数列表
    static std::vector<CallLaterFunction> callLaterFunctionList;
    
    // 异步函数列表
    static std::vector<CallLaterFunction> callAsyncFunctionList;

    void callLater(CallLaterFunction func) {
        if (func) {
            callLaterFunctionList.push_back(func);
        }
    }
    
    void callAsync(CallLaterFunction func) {
        if (func) {
            callAsyncFunctionList.push_back(func);
        }
    }
    
    void executeLaters() {
        if (callLaterFunctionList.empty()) {
            return;
        }
        
        // 复制列表以避免在执行过程中的并发修改问题
        auto functionList = std::move(callLaterFunctionList);
        callLaterFunctionList.clear();
        
        // 执行所有延迟函数
        for (auto& func : functionList) {
            try {
                if (func) {
                    func();
                }
            }
            catch (const std::exception& e) {
                EGRET_ERRORF("callLater执行出错: {}", e.what());
            }
        }
    }
    
    void executeAsyncs() {
        if (callAsyncFunctionList.empty()) {
            return;
        }
        
        // 复制列表以避免在执行过程中的并发修改问题
        auto functionList = std::move(callAsyncFunctionList);
        callAsyncFunctionList.clear();
        
        // 执行所有异步函数
        for (auto& func : functionList) {
            try {
                if (func) {
                    func();
                }
            }
            catch (const std::exception& e) {
                EGRET_ERRORF("callAsync执行出错: {}", e.what());
            }
        }
    }
    
    void clear() {
        callLaterFunctionList.clear();
        callAsyncFunctionList.clear();
    }

} // namespace CallLaterSystem
} // namespace egret
