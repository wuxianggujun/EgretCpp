#include "utils/CallLater.hpp"
#include <iostream>

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
                std::cerr << "Error in callLater function: " << e.what() << std::endl;
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
                std::cerr << "Error in callAsync function: " << e.what() << std::endl;
            }
        }
    }
    
    void clear() {
        callLaterFunctionList.clear();
        callAsyncFunctionList.clear();
    }

} // namespace CallLaterSystem
} // namespace egret