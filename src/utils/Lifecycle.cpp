#include "utils/Lifecycle.hpp"
#include "events/Event.hpp"
#include <iostream>

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
            std::cerr << "Error in lifecycle plugin: " << e.what() << std::endl;
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
                std::cerr << "Error in pause callback: " << e.what() << std::endl;
            }
        }
        
        // 暂停所有上下文
        for (auto& context : m_contexts) {
            if (context) {
                context->pause();
            }
        }
        
        std::cout << "Lifecycle paused" << std::endl;
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
                std::cerr << "Error in resume callback: " << e.what() << std::endl;
            }
        }
        
        // 恢复所有上下文
        for (auto& context : m_contexts) {
            if (context) {
                context->resume();
            }
        }
        
        std::cout << "Lifecycle resumed" << std::endl;
    }

} // namespace egret