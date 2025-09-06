#include "utils/Lifecycle.hpp"
#include "events/Event.hpp"
#include <iostream>
#include "utils/Logger.hpp"

namespace egret {

    void LifecycleContext::pause() {
        // 子类可以重写此方法
    }
    
    void LifecycleContext::resume() {
        // 子类可以重写此方法
    }
    
    LifecycleManager& LifecycleManager::getInstance() {
        static LifecycleManager instance;
        return instance;
    }
    
    void LifecycleManager::addLifecycleListener(LifecyclePlugin plugin) {
        if (!plugin) {
            return;
        }
        
        auto context = std::make_shared<LifecycleContext>();
        m_contexts.push_back(context);
        
        try {
            plugin(context);
        }
        catch (const std::exception& e) {
            EGRET_ERRORF("Lifecycle插件错误: {}", e.what());
        }
    }
    
    void LifecycleManager::pauseAll() {
        if (!m_isActivated) {
            return;
        }
        
        m_isActivated = false;
        
        // 派发DEACTIVATE事件
        if (m_stage) {
            auto event = std::make_shared<Event>(Event::DEACTIVATE);
            m_stage->dispatchEvent(*event);
        }
        
        // 调用暂停回调
        if (m_onPause) {
            try {
                m_onPause();
            }
            catch (const std::exception& e) {
                EGRET_ERRORF("pause回调错误: {}", e.what());
            }
        }
        
        // 暂停所有上下文
        for (auto& context : m_contexts) {
            if (context) {
                context->pause();
            }
        }
        
        EGRET_INFO("Lifecycle暂停");
    }
    
    void LifecycleManager::resumeAll() {
        if (m_isActivated) {
            return;
        }
        
        m_isActivated = true;
        
        // 派发ACTIVATE事件
        if (m_stage) {
            auto event = std::make_shared<Event>(Event::ACTIVATE);
            m_stage->dispatchEvent(*event);
        }
        
        // 调用恢复回调
        if (m_onResume) {
            try {
                m_onResume();
            }
            catch (const std::exception& e) {
                EGRET_ERRORF("resume回调错误: {}", e.what());
            }
        }
        
        // 恢复所有上下文
        for (auto& context : m_contexts) {
            if (context) {
                context->resume();
            }
        }
        
        EGRET_INFO("Lifecycle恢复");
    }

} // namespace egret
