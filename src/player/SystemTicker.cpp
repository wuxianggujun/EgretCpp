#include "player/SystemTicker.hpp"
#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "display/Stage.hpp"
#include "events/Event.hpp"
#include "utils/Timer.hpp"
#include "utils/CallLater.hpp"
#include "utils/Lifecycle.hpp"
#include <algorithm>
#include <cmath>
#include "utils/Logger.hpp"

namespace egret {
namespace sys {

    SystemTicker::SystemTicker() 
        : m_frameRate(30)
        , m_frameInterval(0)
        , m_frameDeltaTime(0)
        , m_lastTimeStamp(0)
        , m_lastCount(0)
        , m_costEnterFrame(0)
        , m_isPaused(false) {
        
        // 初始化时间
        START_TIME = getTimer();
        
        // 计算帧相关参数
        m_frameDeltaTime = 1000.0 / m_frameRate;
        m_lastCount = m_frameInterval = static_cast<int>(std::round(60000.0 / m_frameRate));
        
        EGRET_INFOF("SystemTicker初始化: frameRate={}", m_frameRate);
    }
    
    void SystemTicker::addPlayer(std::shared_ptr<Player> player) {
        if (!player) {
            return;
        }
        
        // 检查是否已存在
        auto it = std::find(m_playerList.begin(), m_playerList.end(), player);
        if (it != m_playerList.end()) {
            return;
        }
        
        // 添加到播放器列表
        m_playerList.push_back(player);
        
        EGRET_DEBUGF("添加Player，当前数量={}", m_playerList.size());
    }
    
    void SystemTicker::removePlayer(std::shared_ptr<Player> player) {
        if (!player) {
            return;
        }
        
        auto it = std::find(m_playerList.begin(), m_playerList.end(), player);
        if (it != m_playerList.end()) {
            m_playerList.erase(it);
            EGRET_DEBUGF("移除Player，剩余数量={}", m_playerList.size());
        }
    }
    
    void SystemTicker::startTick(std::function<bool(long long)> callback) {
        if (!callback) {
            return;
        }
        
        // 检查是否已存在（简化版本，实际应该更精确）
        int index = getTickIndex(callback);
        if (index != -1) {
            return;
        }
        
        // 添加回调
        TickCallback tickCallback;
        tickCallback.callback = callback;
        m_callBackList.push_back(tickCallback);
        
        EGRET_DEBUGF("启动tick回调，数量={}", m_callBackList.size());
    }
    
    void SystemTicker::stopTick(std::function<bool(long long)> callback) {
        if (!callback) {
            return;
        }
        
        int index = getTickIndex(callback);
        if (index == -1) {
            return;
        }
        
        m_callBackList.erase(m_callBackList.begin() + index);
        
        EGRET_DEBUGF("停止tick回调，剩余数量={}", m_callBackList.size());
    }
    
    int SystemTicker::getTickIndex(const std::function<bool(long long)>& callback) const {
        // 简化版本：在实际实现中，可能需要更复杂的比较逻辑
        // 因为std::function的比较比较复杂
        return -1; // 暂时返回-1，表示未找到
    }
    
    bool SystemTicker::setFrameRate(int value) {
        if (value <= 0) {
            return false;
        }
        
        if (m_frameRate == value) {
            return false;
        }
        
        m_frameRate = value;
        
        // 限制最大帧率为60
        if (value > 60) {
            value = 60;
        }
        
        m_frameDeltaTime = 1000.0 / value;
        // 使用60*1000来避免浮点数计算不准确的问题
        m_lastCount = m_frameInterval = static_cast<int>(std::round(60000.0 / value));
        
        EGRET_INFOF("设置帧率={} (限制为{})", m_frameRate, value);
        return true;
    }
    
    void SystemTicker::pause() {
        m_isPaused = true;
        EGRET_INFO("SystemTicker暂停");
    }
    
    void SystemTicker::resume() {
        m_isPaused = false;
        EGRET_INFO("SystemTicker恢复");
    }
    
    void SystemTicker::update(bool forceUpdate) {
        long long t1 = getTimer();
        
        bool requestRenderingFlag = egret::sys::requestRenderingFlag;
        long long timeStamp = getTimer();
        
        // 更新生命周期上下文
        auto& lifecycle = LifecycleManager::getInstance();
        const auto& contexts = lifecycle.getContexts();
        for (const auto& context : contexts) {
            if (context && context->onUpdate) {
                context->onUpdate();
            }
        }
        
        // 如果暂停，只更新时间戳
        if (m_isPaused) {
            m_lastTimeStamp = timeStamp;
            return;
        }
        
        // 执行异步调用
        CallLaterSystem::executeAsyncs();
        
        // 执行tick回调
        for (auto& tickCallback : m_callBackList) {
            if (tickCallback.callback && tickCallback.callback(timeStamp)) {
                requestRenderingFlag = true;
            }
        }
        
        long long t2 = getTimer();
        long long deltaTime = timeStamp - m_lastTimeStamp;
        m_lastTimeStamp = timeStamp;
        
        // 帧率控制逻辑
        if (deltaTime >= m_frameDeltaTime || forceUpdate) {
            m_lastCount = m_frameInterval;
        } else {
            m_lastCount -= 1000;
            if (m_lastCount > 0) {
                if (requestRenderingFlag) {
                    render(false, m_costEnterFrame + t2 - t1);
                }
                return;
            }
            m_lastCount += m_frameInterval;
        }
        
        // 执行渲染
        render(true, m_costEnterFrame + t2 - t1);
        
        // 广播EnterFrame事件
        long long t3 = getTimer();
        broadcastEnterFrame();
        long long t4 = getTimer();
        m_costEnterFrame = t4 - t3;
    }
    
    void SystemTicker::render(bool triggerByFrame, long long costTicker) {
        if (m_playerList.empty()) {
            return;
        }
        
        // 执行延迟调用
        CallLaterSystem::executeLaters();
        
        // 广播Render事件
        if (invalidateRenderFlag) {
            broadcastRender();
            invalidateRenderFlag = false;
        }
        
        // 渲染所有播放器
        for (auto& player : m_playerList) {
            if (player) {
                player->render(triggerByFrame, static_cast<int>(costTicker));
            }
        }
        
        requestRenderingFlag = false;
    }
    
    void SystemTicker::broadcastEnterFrame() {
        if (m_playerList.empty()) {
            return;
        }

        // 深度优先遍历显示树，向每个显示对象派发 ENTER_FRAME
        auto dispatchTree = [](egret::DisplayObject* obj, egret::Event& evt, auto&& dispatchTreeRef) -> void {
            if (!obj) return;
            obj->dispatchEvent(evt);
            if (auto container = dynamic_cast<egret::DisplayObjectContainer*>(obj)) {
                int n = container->getNumChildren();
                for (int i = 0; i < n; ++i) {
                    dispatchTreeRef(container->getChildAt(i), evt, dispatchTreeRef);
                }
            }
        };

        egret::Event enter(Event::ENTER_FRAME);
        for (auto& player : m_playerList) {
            if (!player) continue;
            auto stage = player->getStage();
            if (!stage) continue;
            dispatchTree(stage.get(), enter, dispatchTree);
        }
    }
    
    void SystemTicker::broadcastRender() {
        // TODO: 实现Render事件广播  
        // 这需要DisplayObject的静态回调列表支持
        // 暂时留空，等DisplayObject系统完善后实现
        
        /* 原TypeScript代码：
        let list = DisplayObject.$renderCallBackList;
        let length = list.length;
        if (length == 0) {
            return;
        }
        list = list.concat();
        for (let i = 0; i < length; i++) {
            list[i].dispatchEventWith(Event.RENDER);
        }
        */
    }
    
    // 全局ticker实例
    SystemTicker& getTicker() {
        static SystemTicker instance;
        return instance;
    }

} // namespace sys
} // namespace egret
