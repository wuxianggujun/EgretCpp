#pragma once

#include "platform/sdl/SDLWindow.hpp"
#include "platform/sdl/SDLEventConverter.hpp"
#include "player/Player.hpp"
#include "player/SystemTicker.hpp"
#include <SDL3/SDL.h>
#include <memory>

namespace egret {
namespace platform {

    /**
     * SDL平台管理器
     * 集成SDL3窗口管理、事件处理和引擎系统
     */
    class SDLPlatform {
    public:
        /**
         * 构造函数
         */
        SDLPlatform();
        
        /**
         * 析构函数
         */
        ~SDLPlatform();
        
        /**
         * 禁止拷贝
         */
        SDLPlatform(const SDLPlatform&) = delete;
        SDLPlatform& operator=(const SDLPlatform&) = delete;
        
        /**
         * 初始化SDL平台
         * @param width 窗口宽度
         * @param height 窗口高度
         * @param title 窗口标题
         * @return 是否初始化成功
         */
        bool initialize(int width, int height, const std::string& title = "Egret Player");
        
        /**
         * 创建播放器
         * @param entryClassName 入口类名
         * @return 播放器实例
         */
        std::shared_ptr<sys::Player> createPlayer(const std::string& entryClassName);
        
        /**
         * 运行主循环
         * @return 退出代码
         */
        int runMainLoop();
        
        /**
         * 停止主循环
         */
        void stop();
        
        /**
         * 获取SDL窗口
         */
        std::shared_ptr<SDLWindow> getWindow() const { return m_window; }
        
        /**
         * 获取播放器
         */
        std::shared_ptr<sys::Player> getPlayer() const { return m_player; }
        
        /**
         * 是否初始化成功
         */
        bool isInitialized() const { return m_initialized; }
        
    private:
        /**
         * 处理事件
         */
        void processEvents();
        
        /**
         * 更新引擎
         */
        void updateEngine();
        
        /**
         * 渲染帧
         */
        void render();
        
        std::shared_ptr<SDLWindow> m_window;              // SDL窗口管理器
        std::shared_ptr<SDLEventConverter> m_eventConverter; // 事件转换器
        std::shared_ptr<sys::Player> m_player;            // 播放器实例
        sys::SystemTicker* m_ticker;                      // 系统Ticker
        bool m_initialized;                               // 是否已初始化
        bool m_running;                                   // 是否正在运行
    };

} // namespace platform
} // namespace egret