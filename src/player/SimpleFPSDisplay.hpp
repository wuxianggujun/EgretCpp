#pragma once

#include "player/Player.hpp"
#include "display/Stage.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace egret {
namespace sys {

    /**
     * 简单的FPS显示实现
     * 对应 TypeScript 的 FPSImpl 类
     */
    class SimpleFPSDisplay : public FPSDisplay {
    public:
        /**
         * 构造函数
         * @param stage 舞台对象
         * @param showFPS 是否显示FPS
         * @param showLog 是否显示日志
         * @param logFilter 日志过滤器
         */
        SimpleFPSDisplay(std::shared_ptr<Stage> stage, bool showFPS, bool showLog, const std::string& logFilter = "");
        
        /**
         * 析构函数
         */
        virtual ~SimpleFPSDisplay() = default;
        
        /**
         * 更新FPS信息
         */
        void update(int fps, int drawCalls, int costRender, int costTicker) override;
        
        /**
         * 更新信息日志
         */
        void updateInfo(const std::string& info) override;
        
        /**
         * 更新警告日志
         */
        void updateWarn(const std::string& info) override;
        
        /**
         * 更新错误日志
         */
        void updateError(const std::string& info) override;
        
    private:
        /**
         * 过滤日志消息
         */
        bool filterMessage(const std::string& message) const;
        
        /**
         * 输出FPS信息到控制台
         */
        void outputFPSInfo() const;
        
        // ========== 私有成员变量 ==========
        
        std::shared_ptr<Stage> m_stage;           // 舞台引用
        bool m_showFPS;                           // 显示FPS
        bool m_showLog;                           // 显示日志
        std::string m_logFilter;                  // 日志过滤器
        
        // FPS统计相关
        std::vector<std::string> m_infoLines;     // 信息日志行
        long long m_totalTime;                    // 总时间
        int m_totalTick;                          // 总tick数
        long long m_lastTime;                     // 上次时间
        int m_drawCalls;                          // 绘制调用次数
        int m_costRender;                         // 渲染消耗时间
        int m_costTicker;                         // ticker消耗时间
        
        // 当前显示的FPS数据
        int m_currentFPS;
        int m_currentDrawCalls;
        int m_currentCostRender;
        int m_currentCostTicker;
    };

} // namespace sys
} // namespace egret