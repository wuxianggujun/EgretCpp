#include "platform/sdl/SDLWindow.hpp"
#include <iostream>
#include <stdexcept>

namespace egret {
namespace platform {

    SDLWindow::SDLWindow(int width, int height, const std::string& title)
        : m_window(nullptr)
        , m_renderer(nullptr)
        , m_shouldClose(false)
        , m_width(width)
        , m_height(height)
        , m_title(title) {
        
        // 创建SDL窗口
        m_window = SDL_CreateWindow(
            title.c_str(),
            width, height,
            SDL_WINDOW_RESIZABLE
        );
        
        if (!m_window) {
            throw std::runtime_error(std::string("Failed to create SDL window: ") + SDL_GetError());
        }
        
        // 创建SDL渲染器
        m_renderer = SDL_CreateRenderer(
            m_window,
            nullptr
        );
        
        if (!m_renderer) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            throw std::runtime_error(std::string("Failed to create SDL renderer: ") + SDL_GetError());
        }
        
        // 设置渲染器混合模式
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
        
        std::cout << "SDL Window created: " << width << "x" << height << " - " << title << std::endl;
    }
    
    SDLWindow::~SDLWindow() {
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }
        
        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
        
        std::cout << "SDL Window destroyed" << std::endl;
    }
    
    bool SDLWindow::pollEvents() {
        if (SDL_PollEvent(&m_currentEvent)) {
            // 处理窗口关闭事件
            if (m_currentEvent.type == SDL_EVENT_QUIT) {
                m_shouldClose = true;
            }
            // 处理窗口大小改变事件
            else if (m_currentEvent.type == SDL_EVENT_WINDOW_RESIZED) {
                m_width = m_currentEvent.window.data1;
                m_height = m_currentEvent.window.data2;
                std::cout << "Window resized to: " << m_width << "x" << m_height << std::endl;
            }
            
            return true;
        }
        
        return false;
    }
    
    void SDLWindow::present() {
        if (m_renderer) {
            SDL_RenderPresent(m_renderer);
        }
    }
    
    void SDLWindow::clear() {
        if (m_renderer) {
            // 设置清除颜色为黑色
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderClear(m_renderer);
        }
    }
    
    void SDLWindow::getSize(int& width, int& height) const {
        width = m_width;
        height = m_height;
    }
    
    void SDLWindow::setSize(int width, int height) {
        if (m_window && (width != m_width || height != m_height)) {
            SDL_SetWindowSize(m_window, width, height);
            m_width = width;
            m_height = height;
            std::cout << "Window size set to: " << width << "x" << height << std::endl;
        }
    }
    
    std::string SDLWindow::getTitle() const {
        return m_title;
    }
    
    void SDLWindow::setTitle(const std::string& title) {
        if (m_window && title != m_title) {
            SDL_SetWindowTitle(m_window, title.c_str());
            m_title = title;
            std::cout << "Window title set to: " << title << std::endl;
        }
    }

} // namespace platform
} // namespace egret