#include "platform/sdl/SDLEventConverter.hpp"
#include "events/TouchEvent.hpp"
#include "events/Event.hpp"
#include "events/KeyboardEvent.hpp"
#include <iostream>
#include "utils/Logger.hpp"

namespace egret {
namespace platform {

    SDLEventConverter::SDLEventConverter(std::shared_ptr<Stage> stage) 
        : m_stage(stage) {
    }
    
    bool SDLEventConverter::handleSDLEvent(const SDL_Event& sdlEvent) {
        if (!m_stage) {
            return false;
        }
        
        switch (sdlEvent.type) {
            // 鼠标事件
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_MOTION:
                return handleMouseEvent(sdlEvent);
                
            // 触摸事件
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION:
                return handleTouchEvent(sdlEvent);
                
            // 键盘事件
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                return handleKeyboardEvent(sdlEvent);
                
            // 窗口事件
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                return handleWindowEvent(sdlEvent);
                
            default:
                return false;
        }
    }
    
    bool SDLEventConverter::handleMouseEvent(const SDL_Event& sdlEvent) {
        float x = static_cast<float>(sdlEvent.button.x);
        float y = static_cast<float>(sdlEvent.button.y);
        convertCoordinates(x, y);
        
        std::shared_ptr<TouchEvent> touchEvent;
        
        switch (sdlEvent.type) {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                    touchEvent = createTouchEvent(TouchEvent::TOUCH_BEGIN, x, y);
                    EGRET_DEBUGF("Mouse->TOUCH_BEGIN ({}, {})", x, y);
                }
                break;
                
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                    touchEvent = createTouchEvent(TouchEvent::TOUCH_END, x, y);
                    EGRET_DEBUGF("Mouse->TOUCH_END ({}, {})", x, y);
                }
                break;
                
            case SDL_EVENT_MOUSE_MOTION:
                // 只有在按下鼠标左键时才转换为触摸移动
                if (sdlEvent.motion.state & SDL_BUTTON_LMASK) {
                    x = static_cast<float>(sdlEvent.motion.x);
                    y = static_cast<float>(sdlEvent.motion.y);
                    convertCoordinates(x, y);
                    touchEvent = createTouchEvent(TouchEvent::TOUCH_MOVE, x, y);
                    // 不打印移动事件，避免日志过多
                }
                break;
        }
        
        if (touchEvent) {
            m_stage->dispatchEvent(*touchEvent);
            return true;
        }
        
        return false;
    }
    
    bool SDLEventConverter::handleTouchEvent(const SDL_Event& sdlEvent) {
        float x = sdlEvent.tfinger.x;
        float y = sdlEvent.tfinger.y;
        int touchId = static_cast<int>(sdlEvent.tfinger.fingerID);
        
        // SDL触摸坐标是归一化的(0-1)，需要转换为实际像素坐标
        x *= static_cast<float>(m_stage->getStageWidth());
        y *= static_cast<float>(m_stage->getStageHeight());
        
        std::shared_ptr<TouchEvent> touchEvent;
        
        switch (sdlEvent.type) {
            case SDL_EVENT_FINGER_DOWN:
                touchEvent = createTouchEvent(TouchEvent::TOUCH_BEGIN, x, y, touchId);
                EGRET_DEBUGF("Finger DOWN ({}, {}), id={}", x, y, touchId);
                break;
                
            case SDL_EVENT_FINGER_UP:
                touchEvent = createTouchEvent(TouchEvent::TOUCH_END, x, y, touchId);
                EGRET_DEBUGF("Finger UP ({}, {}), id={}", x, y, touchId);
                break;
                
            case SDL_EVENT_FINGER_MOTION:
                touchEvent = createTouchEvent(TouchEvent::TOUCH_MOVE, x, y, touchId);
                break;
        }
        
        if (touchEvent) {
            m_stage->dispatchEvent(*touchEvent);
            return true;
        }
        
        return false;
    }
    
    bool SDLEventConverter::handleKeyboardEvent(const SDL_Event& sdlEvent) {
        if (!m_stage) {
            return false;
        }
        
        // 创建键盘事件
        auto keyboardEvent = KeyboardEvent::createFromSDLEvent(sdlEvent);
        if (!keyboardEvent) {
            return false;
        }
        
        // 输出调试信息
        std::string eventType = (sdlEvent.type == SDL_EVENT_KEY_DOWN) ? "KEY_DOWN" : "KEY_UP";
        EGRET_DEBUGF("Keyboard {}: key={} code={} char={}{}{}{}", eventType,
                     keyboardEvent->getKeyName(), keyboardEvent->getKeyCode(), keyboardEvent->getCharCode(),
                     keyboardEvent->getCtrlKey() ? " [Ctrl]" : "",
                     keyboardEvent->getAltKey() ? " [Alt]" : "",
                     keyboardEvent->getShiftKey() ? " [Shift]" : "");
        
        // 派发事件到舞台
        bool result = m_stage->dispatchEvent(*keyboardEvent);
        
        return result;
    }
    
    bool SDLEventConverter::handleWindowEvent(const SDL_Event& sdlEvent) {
        std::shared_ptr<Event> event;
        
        switch (sdlEvent.type) {
            case SDL_EVENT_WINDOW_RESIZED: {
                // 同步舞台与渲染缓冲尺寸
                int newWidth = sdlEvent.window.data1;
                int newHeight = sdlEvent.window.data2;
                // 统一调用Stage::resize以确保DisplayList的RenderBuffer同步调整
                m_stage->resize(newWidth, newHeight);

                // 派发RESIZE事件
                event = std::make_shared<Event>(Event::RESIZE);
                EGRET_INFOF("窗口Resize事件: {}x{}", newWidth, newHeight);
                break;
            }
                
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                event = std::make_shared<Event>(Event::ACTIVATE);
                EGRET_INFO("窗口获得焦点，派发ACTIVATE事件");
                break;
                
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                event = std::make_shared<Event>(Event::DEACTIVATE);
                EGRET_INFO("窗口失去焦点，派发DEACTIVATE事件");
                break;
        }
        
        if (event) {
            m_stage->dispatchEvent(*event);
            return true;
        }
        
        return false;
    }
    
    std::shared_ptr<TouchEvent> SDLEventConverter::createTouchEvent(
        const std::string& type, float x, float y, int touchId) {
        
        auto touchEvent = std::make_shared<TouchEvent>(type);
        
        // TODO: 当TouchEvent实现后，设置这些属性
        // touchEvent->setStageX(x);
        // touchEvent->setStageY(y);
        // touchEvent->setTouchPointID(touchId);
        
        return touchEvent;
    }
    
    void SDLEventConverter::convertCoordinates(float& x, float& y) const {
        // 在这里可以添加坐标系转换逻辑
        // 比如处理不同的缩放模式、DPI缩放等
        // 暂时保持原样
    }

} // namespace platform
} // namespace egret
