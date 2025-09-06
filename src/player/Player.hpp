#pragma once

#include "display/Stage.hpp"
#include "display/DisplayList.hpp"
#include "display/DisplayObject.hpp"
#include "player/RenderBuffer.hpp"
#include "player/PlayerOption.hpp"
#include "platform/sdl/SDLWindow.hpp"
#include "platform/sdl/SDLEventConverter.hpp"
#include "../core/HashObject.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <string>
#include <functional>

namespace egret {
namespace sys {

    // 前向声明
    class Ticker;
    
    /**
     * FPS显示接口
     */
    class FPSDisplay {
    public:
        virtual ~FPSDisplay() = default;
        
        /**
         * 更新FPS信息
         */
        virtual void update(int fps, int drawCalls, int costRender, int costTicker) = 0;
        
        /**
         * 更新信息日志
         */
        virtual void updateInfo(const std::string& info) = 0;
        
        /**
         * 更新警告日志
         */
        virtual void updateWarn(const std::string& info) = 0;
        
        /**
         * 更新错误日志
         */
        virtual void updateError(const std::string& info) = 0;
    };

    /**
     * Egret播放器
     * 对应 TypeScript 的 egret.sys.Player 类
     * 现在集成了SDL3窗口管理
     */
    class Player : public HashObject, public std::enable_shared_from_this<Player> {
    public:
        /**
         * 构造函数（带SDL窗口）
         * @param width 窗口宽度
         * @param height 窗口高度
         * @param title 窗口标题
         * @param entryClassName 入口类名
         */
        Player(int width, int height, const std::string& title = "Egret Player", const std::string& entryClassName = "");
        
        /**
         * 构造函数（传统方式，兼容现有代码）
         * @param buffer 渲染缓冲区
         * @param stage 舞台对象
         * @param entryClassName 入口类名
         */
        Player(std::shared_ptr<RenderBuffer> buffer, std::shared_ptr<Stage> stage, const std::string& entryClassName);
        
        /**
         * 析构函数
         */
        virtual ~Player();
        
        /**
         * 启动播放器
         */
        void start();
        
        /**
         * 停止播放器
         */
        void stop();
        
        /**
         * 暂停播放器
         */
        void pause();
        
        /**
         * 渲染屏幕
         * @param triggerByFrame 是否由帧触发
         * @param costTicker ticker消耗时间
         */
        void render(bool triggerByFrame, int costTicker);
        
        /**
         * 更新舞台尺寸
         * @param stageWidth 舞台宽度
         * @param stageHeight 舞台高度
         */
        void updateStageSize(int stageWidth, int stageHeight);
        
        /**
         * 显示FPS和日志
         * @param showFPS 是否显示FPS
         * @param showLog 是否显示日志
         * @param logFilter 日志过滤器
         */
        void displayFPS(bool showFPS, bool showLog, const std::string& logFilter = "");
        
        /**
         * 获取舞台对象
         */
        std::shared_ptr<Stage> getStage() const { return m_stage; }
        
        /**
         * 获取根显示对象
         */
        std::shared_ptr<DisplayObject> getRoot() const { return m_root; }
        
        /**
         * 是否正在播放
         */
        bool isPlaying() const { return m_isPlaying; }
        
        /**
         * 设置入口类工厂函数
         */
        void setEntryClassFactory(std::function<std::shared_ptr<DisplayObject>()> factory) {
            m_entryClassFactory = factory;
        }
        
        /**
         * 获取SDL窗口
         */
        std::shared_ptr<platform::SDLWindow> getSDLWindow() const { return m_sdlWindow; }
        
        /**
         * 处理SDL事件
         * @param sdlEvent SDL原生事件
         */
        void handleSDLEvent(const SDL_Event& sdlEvent);
        
        /**
         * 运行主循环（当Player拥有SDL窗口时）
         * @return 退出代码
         */
        int runMainLoop();
        
    private:
        /**
         * 初始化播放器
         */
        void initialize();
        
        /**
         * 创建显示列表
         */
        std::shared_ptr<DisplayList> createDisplayList(std::shared_ptr<Stage> stage, std::shared_ptr<RenderBuffer> buffer);
        
        /**
         * 日志回调函数类型
         */
        using LogCallback = std::function<void(const std::string&)>;
        
        // ========== 私有成员变量 ==========
        
        std::shared_ptr<DisplayList> m_screenDisplayList;     // 屏幕显示列表
        std::string m_entryClassName;                         // 入口类名
        std::shared_ptr<Stage> m_stage;                       // 舞台引用
        std::shared_ptr<DisplayObject> m_root;                // 根显示对象
        bool m_isPlaying;                                     // 是否正在播放
        
        // FPS和日志显示相关
        bool m_showFPS;                                       // 显示FPS
        bool m_showLog;                                       // 显示日志
        std::shared_ptr<FPSDisplay> m_fpsDisplay;             // FPS显示对象
        
        // 入口类工厂函数
        std::function<std::shared_ptr<DisplayObject>()> m_entryClassFactory;
        
        // 性能统计
        int m_lastDrawCalls;                                  // 上次绘制调用次数
        long long m_lastRenderTime;                           // 上次渲染时间
        
        // SDL集成相关
        std::shared_ptr<platform::SDLWindow> m_sdlWindow;     // SDL窗口管理器
        std::shared_ptr<platform::SDLEventConverter> m_eventConverter; // 事件转换器
        bool m_ownWindow;                                     // 是否拥有窗口（用于析构时判断）

        // SDL呈现纹理（将Skia渲染结果拷贝后显示到窗口）
        SDL_Texture* m_presentTexture = nullptr;
        int m_presentTexW = 0;
        int m_presentTexH = 0;
    };

} // namespace sys
} // namespace egret
