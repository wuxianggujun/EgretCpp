#include "player/SimpleFPSDisplay.hpp"
#include <iostream>
#include <algorithm>
#include <regex>
#include <iomanip>

namespace egret {
namespace sys {

    SimpleFPSDisplay::SimpleFPSDisplay(std::shared_ptr<Stage> stage, bool showFPS, bool showLog, const std::string& logFilter)
        : m_stage(stage)
        , m_showFPS(showFPS)
        , m_showLog(showLog)
        , m_logFilter(logFilter)
        , m_totalTime(0)
        , m_totalTick(0)
        , m_lastTime(0)
        , m_drawCalls(0)
        , m_costRender(0)
        , m_costTicker(0)
        , m_currentFPS(0)
        , m_currentDrawCalls(0)
        , m_currentCostRender(0)
        , m_currentCostTicker(0) {
        
        // 初始化时间
        m_lastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void SimpleFPSDisplay::update(int fps, int drawCalls, int costRender, int costTicker) {
        auto current = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
        m_totalTime += current - m_lastTime;
        m_lastTime = current;
        m_totalTick++;
        m_drawCalls += drawCalls;
        m_costRender += costRender;
        m_costTicker += costTicker;
        
        // 每秒更新一次显示
        if (m_totalTime >= 1000) {
            // 计算平均值
            int avgFPS = std::min(static_cast<int>(m_totalTick * 1000 / m_totalTime), 60); // 假设最大帧率60
            int avgDrawCalls = m_totalTick > 0 ? m_drawCalls / m_totalTick : 0;
            int avgCostRender = m_totalTick > 0 ? m_costRender / m_totalTick : 0;
            int avgCostTicker = m_totalTick > 0 ? m_costTicker / m_totalTick : 0;
            
            // 更新当前显示数据
            m_currentFPS = avgFPS;
            m_currentDrawCalls = avgDrawCalls;
            m_currentCostRender = avgCostRender;
            m_currentCostTicker = avgCostTicker;
            
            // 输出FPS信息
            if (m_showFPS) {
                outputFPSInfo();
            }
            
            // 重置统计数据
            m_totalTick = 0;
            m_totalTime = m_totalTime % 1000;
            m_drawCalls = 0;
            m_costRender = 0;
            m_costTicker = 0;
        }
    }
    
    void SimpleFPSDisplay::updateInfo(const std::string& info) {
        if (!m_showLog || info.empty()) {
            return;
        }
        
        if (!filterMessage(info)) {
            return;
        }
        
        std::cout << "[INFO] " << info << std::endl;
        
        // 保存到历史记录（限制数量）
        m_infoLines.push_back("[INFO] " + info);
        if (m_infoLines.size() > 100) { // 限制最大日志行数
            m_infoLines.erase(m_infoLines.begin());
        }
    }
    
    void SimpleFPSDisplay::updateWarn(const std::string& info) {
        if (!m_showLog || info.empty()) {
            return;
        }
        
        if (!filterMessage(info)) {
            return;
        }
        
        std::cout << "\033[33m[WARN] " << info << "\033[0m" << std::endl; // 黄色输出
        
        m_infoLines.push_back("[WARN] " + info);
        if (m_infoLines.size() > 100) {
            m_infoLines.erase(m_infoLines.begin());
        }
    }
    
    void SimpleFPSDisplay::updateError(const std::string& info) {
        if (!m_showLog || info.empty()) {
            return;
        }
        
        if (!filterMessage(info)) {
            return;
        }
        
        std::cout << "\033[31m[ERROR] " << info << "\033[0m" << std::endl; // 红色输出
        
        m_infoLines.push_back("[ERROR] " + info);
        if (m_infoLines.size() > 100) {
            m_infoLines.erase(m_infoLines.begin());
        }
    }
    
    bool SimpleFPSDisplay::filterMessage(const std::string& message) const {
        if (m_logFilter.empty()) {
            return true;
        }
        
        try {
            // 尝试作为正则表达式匹配
            std::regex filterRegex(m_logFilter);
            return std::regex_search(message, filterRegex);
        }
        catch (const std::exception&) {
            // 如果正则表达式无效，使用简单的字符串匹配
            return message.find(m_logFilter) == 0; // 检查是否以过滤器开头
        }
    }
    
    void SimpleFPSDisplay::outputFPSInfo() const {
        // 在控制台输出FPS信息
        std::cout << "\033[32m" // 绿色
                  << "=== FPS Info ===" << std::endl
                  << "FPS: " << std::setw(3) << m_currentFPS << " | "
                  << "Draw: " << std::setw(3) << m_currentDrawCalls << " | "
                  << "Render: " << std::setw(3) << m_currentCostRender << "ms | "
                  << "Ticker: " << std::setw(3) << m_currentCostTicker << "ms"
                  << "\033[0m" << std::endl; // 重置颜色
    }

} // namespace sys
} // namespace egret