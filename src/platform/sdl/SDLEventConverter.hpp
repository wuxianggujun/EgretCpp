#pragma once

#include <SDL3/SDL.h>
#include "events/Event.hpp"
#include "events/TouchEvent.hpp"
#include "events/KeyboardEvent.hpp"
#include "display/Stage.hpp"
#include <memory>

namespace egret {
namespace platform {

    /**
     * SDL事件到Egret事件转换器
     * 负责将SDL3的原生事件转换为Egret引擎的事件系统
     */
    class SDLEventConverter {
    public:
        /**
         * 构造函数
         * @param stage 目标舞台对象
         */
        explicit SDLEventConverter(std::shared_ptr<Stage> stage);
        
        /**
         * 析构函数
         */
        ~SDLEventConverter() = default;
        
        /**
         * 处理SDL事件并转换为Egret事件
         * @param sdlEvent SDL原生事件
         * @return 是否成功处理事件
         */
        bool handleSDLEvent(const SDL_Event& sdlEvent);
        
        /**
         * 设置舞台对象
         */
        void setStage(std::shared_ptr<Stage> stage) { m_stage = stage; }
        
        /**
         * 获取舞台对象
         */
        std::shared_ptr<Stage> getStage() const { return m_stage; }
        
    private:
        /**
         * 处理鼠标事件
         */
        bool handleMouseEvent(const SDL_Event& sdlEvent);
        
        /**
         * 处理触摸事件
         */
        bool handleTouchEvent(const SDL_Event& sdlEvent);
        
        /**
         * 处理键盘事件
         */
        bool handleKeyboardEvent(const SDL_Event& sdlEvent);
        
        /**
         * 处理窗口事件
         */
        bool handleWindowEvent(const SDL_Event& sdlEvent);
        
        /**
         * 创建TouchEvent
         */
        std::shared_ptr<TouchEvent> createTouchEvent(const std::string& type, float x, float y, int touchId = 0);
        
        /**
         * 将SDL坐标转换为舞台坐标
         */
        void convertCoordinates(float& x, float& y) const;
        
        std::shared_ptr<Stage> m_stage;  // 目标舞台
    };

} // namespace platform
} // namespace egret