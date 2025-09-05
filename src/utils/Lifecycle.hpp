#pragma once

#include "display/Stage.hpp"
#include "events/Event.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace egret {

    /**
     * 生命周期上下文
     * 对应 TypeScript 的 lifecycle.LifecycleContext
     */
    class LifecycleContext {
    public:
        LifecycleContext() = default;
        virtual ~LifecycleContext() = default;
        
        /**
         * 暂停回调
         */
        virtual void pause();
        
        /**
         * 恢复回调  
         */
        virtual void resume();
        
        /**
         * 更新回调函数
         */
        std::function<void()> onUpdate;
        
    private:
        friend class LifecycleManager;
    };
    
    /**
     * 生命周期插件类型
     */
    using LifecyclePlugin = std::function<void(std::shared_ptr<LifecycleContext>)>;

    /**
     * 生命周期管理器
     * 对应 TypeScript 的 lifecycle 模块
     */
    class LifecycleManager {
    public:
        /**
         * 获取单例实例
         */
        static LifecycleManager& getInstance();
        
        /**
         * 设置舞台
         */
        void setStage(std::shared_ptr<Stage> stage) { m_stage = stage; }
        
        /**
         * 获取舞台
         */
        std::shared_ptr<Stage> getStage() const { return m_stage; }
        
        /**
         * 添加生命周期监听器
         */
        void addLifecycleListener(LifecyclePlugin plugin);
        
        /**
         * 获取所有上下文
         */
        const std::vector<std::shared_ptr<LifecycleContext>>& getContexts() const {
            return m_contexts;
        }
        
        /**
         * 设置暂停回调
         */
        void setOnPause(std::function<void()> callback) { m_onPause = callback; }
        
        /**
         * 设置恢复回调
         */
        void setOnResume(std::function<void()> callback) { m_onResume = callback; }
        
        /**
         * 暂停所有上下文
         */
        void pauseAll();
        
        /**
         * 恢复所有上下文
         */
        void resumeAll();
        
        /**
         * 是否激活状态
         */
        bool isActivated() const { return m_isActivated; }
        
    private:
        LifecycleManager() = default;
        ~LifecycleManager() = default;
        LifecycleManager(const LifecycleManager&) = delete;
        LifecycleManager& operator=(const LifecycleManager&) = delete;
        
        std::shared_ptr<Stage> m_stage;
        std::vector<std::shared_ptr<LifecycleContext>> m_contexts;
        std::function<void()> m_onPause;
        std::function<void()> m_onResume;
        bool m_isActivated = true;
    };

} // namespace egret