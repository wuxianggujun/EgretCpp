#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <string>

namespace egret {
namespace platform {

    /**
     * SDL3窗口管理类
     * 封装SDL3的窗口和渲染器功能
     */
    class SDLWindow {
    public:
        /**
         * 构造函数
         * @param width 窗口宽度
         * @param height 窗口高度 
         * @param title 窗口标题
         */
        SDLWindow(int width, int height, const std::string& title = "Egret Player");
        
        /**
         * 析构函数
         */
        ~SDLWindow();
        
        /**
         * 禁止拷贝
         */
        SDLWindow(const SDLWindow&) = delete;
        SDLWindow& operator=(const SDLWindow&) = delete;
        
        /**
         * 轮询SDL事件
         * @return 是否还有待处理的事件
         */
        bool pollEvents();
        
        /**
         * 获取最新的SDL事件
         */
        const SDL_Event& getCurrentEvent() const { return m_currentEvent; }
        
        /**
         * 呈现渲染内容到屏幕
         */
        void present();
        
        /**
         * 清空屏幕
         */
        void clear();
        
        /**
         * 获取SDL窗口句柄
         */
        SDL_Window* getWindow() const { return m_window; }
        
        /**
         * 获取SDL渲染器
         */
        SDL_Renderer* getRenderer() const { return m_renderer; }
        
        /**
         * 获取窗口尺寸
         */
        void getSize(int& width, int& height) const;
        
        /**
         * 设置窗口尺寸
         */
        void setSize(int width, int height);
        
        /**
         * 获取窗口标题
         */
        std::string getTitle() const;
        
        /**
         * 设置窗口标题
         */
        void setTitle(const std::string& title);
        
        /**
         * 是否应该关闭窗口
         */
        bool shouldClose() const { return m_shouldClose; }
        
        /**
         * 设置关闭标志
         */
        void setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }
        
        /**
         * 是否初始化成功
         */
        bool isValid() const { return m_window != nullptr && m_renderer != nullptr; }
        
    private:
        SDL_Window* m_window;        // SDL窗口句柄
        SDL_Renderer* m_renderer;    // SDL渲染器句柄
        SDL_Event m_currentEvent;    // 当前事件
        bool m_shouldClose;          // 是否应该关闭窗口
        int m_width;                 // 窗口宽度
        int m_height;                // 窗口高度
        std::string m_title;         // 窗口标题
    };

} // namespace platform
} // namespace egret