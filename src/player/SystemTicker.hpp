#pragma once

#include "player/Player.hpp"
#include "utils/Timer.hpp"
#include "utils/CallLater.hpp"
#include "utils/Lifecycle.hpp"
#include "display/DisplayObject.hpp"
#include "events/Event.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace egret {
namespace sys {

    // 前向声明
    class Player;

    /**
     * Egret心跳计时器
     * 对应 TypeScript 的 egret.sys.SystemTicker
     */
    class SystemTicker {
    public:
        /**
         * 构造函数
         */
        SystemTicker();
        
        /**
         * 析构函数
         */
        ~SystemTicker() = default;
        
        /**
         * 禁止拷贝构造
         */
        SystemTicker(const SystemTicker&) = delete;
        SystemTicker& operator=(const SystemTicker&) = delete;
        
        /**
         * 注册一个播放器实例并运行
         * @param player 播放器实例
         */
        void addPlayer(std::shared_ptr<Player> player);
        
        /**
         * 停止一个播放器实例的运行
         * @param player 播放器实例
         */
        void removePlayer(std::shared_ptr<Player> player);
        
        /**
         * 开始tick回调
         * @param callback tick回调函数，返回true表示需要渲染
         */
        void startTick(std::function<bool(long long)> callback);
        
        /**
         * 停止tick回调
         * @param callback tick回调函数
         */
        void stopTick(std::function<bool(long long)> callback);
        
        /**
         * 设置全局帧率
         * @param value 帧率值
         * @return 是否设置成功
         */
        bool setFrameRate(int value);
        
        /**
         * 获取全局帧率
         */
        int getFrameRate() const { return m_frameRate; }
        
        /**
         * 暂停心跳
         */
        void pause();
        
        /**
         * 恢复心跳
         */
        void resume();
        
        /**
         * 是否暂停
         */
        bool isPaused() const { return m_isPaused; }
        
        /**
         * 执行一次更新
         * @param forceUpdate 是否强制更新
         */
        void update(bool forceUpdate = false);
        
        /**
         * 3D&2D混合渲染：渲染前回调
         */
        std::function<void()> beforeRender;
        
        /**
         * 3D&2D混合渲染：渲染后回调
         */
        std::function<void()> afterRender;
        
    private:
        /**
         * Tick回调信息结构
         */
        struct TickCallback {
            std::function<bool(long long)> callback;
            
            bool operator==(const TickCallback& other) const {
                // 函数指针比较比较复杂，这里简化处理
                return false; // TODO: 更好的比较方式
            }
        };
        
        /**
         * 获取tick回调的索引
         */
        int getTickIndex(const std::function<bool(long long)>& callback) const;
        
        /**
         * 执行一次屏幕渲染
         */
        void render(bool triggerByFrame, long long costTicker);
        
        /**
         * 广播EnterFrame事件
         */
        void broadcastEnterFrame();
        
        /**
         * 广播Render事件
         */
        void broadcastRender();
        
        // ========== 私有成员变量 ==========
        
        std::vector<std::shared_ptr<Player>> m_playerList;    // 播放器列表
        std::vector<TickCallback> m_callBackList;             // tick回调列表
        
        int m_frameRate;                                       // 全局帧率
        int m_frameInterval;                                   // 帧间隔（以60fps为基准的计数）
        double m_frameDeltaTime;                               // 帧时间间隔（毫秒）
        long long m_lastTimeStamp;                             // 上次时间戳
        int m_lastCount;                                       // 剩余计数
        long long m_costEnterFrame;                            // EnterFrame事件消耗时间
        bool m_isPaused;                                       // 是否暂停
        
        // 单例相关
        friend SystemTicker& getTicker();
    };
    
    /**
     * 获取全局ticker实例
     * 对应 TypeScript 的 egret.ticker
     */
    SystemTicker& getTicker();

} // namespace sys
} // namespace egret