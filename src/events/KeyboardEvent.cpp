#include "events/KeyboardEvent.hpp"
#include "events/IEventDispatcher.hpp"
#include <SDL3/SDL.h>
#include <iostream>

namespace egret {

    // ========== 静态成员定义 ==========
    
    const std::string KeyboardEvent::KEY_DOWN = "keyDown";
    const std::string KeyboardEvent::KEY_UP = "keyUp";

    // ========== 构造函数实现 ==========
    
    KeyboardEvent::KeyboardEvent(const std::string& type, bool bubbles, bool cancelable,
                                int keyCode, int charCode, int keyLocation,
                                bool ctrlKey, bool altKey, bool shiftKey)
        : Event(type, bubbles, cancelable)
        , m_keyCode(keyCode)
        , m_charCode(charCode)
        , m_keyLocation(keyLocation)
        , m_ctrlKey(ctrlKey)
        , m_altKey(altKey)
        , m_shiftKey(shiftKey) {
    }

    // ========== SDL3 集成方法实现 ==========
    
    void KeyboardEvent::initFromSDLEvent(const SDL_Event& sdlEvent) {
        if (sdlEvent.type == SDL_EVENT_KEY_DOWN || sdlEvent.type == SDL_EVENT_KEY_UP) {
            const SDL_KeyboardEvent& keyEvent = sdlEvent.key;
            
            // 设置键码值
            m_keyCode = static_cast<int>(keyEvent.key);
            
            // 转换字符码
            m_charCode = convertKeyCodeToCharCode(keyEvent.key, keyEvent.mod);
            
            // 获取按键位置
            m_keyLocation = getKeyLocationFromKeycode(keyEvent.key);
            
            // 设置修饰键状态
            m_ctrlKey = (keyEvent.mod & (SDL_KMOD_LCTRL | SDL_KMOD_RCTRL)) != 0;
            m_altKey = (keyEvent.mod & (SDL_KMOD_LALT | SDL_KMOD_RALT)) != 0;
            m_shiftKey = (keyEvent.mod & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) != 0;
        }
    }
    
    std::string KeyboardEvent::getKeyName() const {
        const char* keyName = SDL_GetKeyName(static_cast<SDL_Keycode>(m_keyCode));
        return keyName ? std::string(keyName) : "Unknown";
    }

    // ========== 静态工厂方法实现 ==========
    
    std::shared_ptr<KeyboardEvent> KeyboardEvent::create(const std::string& type, 
                                                        int keyCode, int charCode, 
                                                        int keyLocation,
                                                        bool ctrlKey, bool altKey, 
                                                        bool shiftKey) {
        return std::make_shared<KeyboardEvent>(type, true, true, keyCode, charCode, 
                                             keyLocation, ctrlKey, altKey, shiftKey);
    }
    
    std::shared_ptr<KeyboardEvent> KeyboardEvent::createFromSDLEvent(const SDL_Event& sdlEvent) {
        if (sdlEvent.type != SDL_EVENT_KEY_DOWN && sdlEvent.type != SDL_EVENT_KEY_UP) {
            return nullptr;
        }
        
        // 确定事件类型
        std::string type = (sdlEvent.type == SDL_EVENT_KEY_DOWN) ? KEY_DOWN : KEY_UP;
        
        // 创建事件实例
        auto keyboardEvent = std::make_shared<KeyboardEvent>(type);
        
        // 从SDL事件初始化
        keyboardEvent->initFromSDLEvent(sdlEvent);
        
        return keyboardEvent;
    }
    
    bool KeyboardEvent::dispatchKeyboardEvent(IEventDispatcher* target, const std::string& type,
                                            int keyCode, int charCode, 
                                            int keyLocation,
                                            bool ctrlKey, bool altKey, 
                                            bool shiftKey) {
        if (!target) {
            return false;
        }
        
        // 创建键盘事件
        auto keyboardEvent = create(type, keyCode, charCode, keyLocation, 
                                  ctrlKey, altKey, shiftKey);
        
        // 派发事件
        bool result = target->dispatchEvent(*keyboardEvent);
        
        return result;
    }

    // ========== 受保护的清理方法实现 ==========
    
    void KeyboardEvent::clean() {
        // 调用父类清理
        Event::clean();
        
        // 重置键盘事件特有的属性
        m_keyCode = 0;
        m_charCode = 0;
        m_keyLocation = KEY_LOCATION_STANDARD;
        m_ctrlKey = false;
        m_altKey = false;
        m_shiftKey = false;
    }

    // ========== 静态辅助方法实现 ==========
    
    int KeyboardEvent::convertKeyCodeToCharCode(SDL_Keycode keycode, SDL_Keymod modifiers) {
        // 处理字母键 (SDL3 使用大写常量)
        if (keycode >= SDLK_A && keycode <= SDLK_Z) {
            if (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) {
                return keycode; // 已经是大写
            }
            return keycode + 32; // 转换为小写
        }
        
        // 处理数字键
        if (keycode >= SDLK_0 && keycode <= SDLK_9) {
            // 如果按下 Shift，返回对应的符号
            if (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) {
                switch (keycode) {
                    case SDLK_1: return '!';
                    case SDLK_2: return '@';
                    case SDLK_3: return '#';
                    case SDLK_4: return '$';
                    case SDLK_5: return '%';
                    case SDLK_6: return '^';
                    case SDLK_7: return '&';
                    case SDLK_8: return '*';
                    case SDLK_9: return '(';
                    case SDLK_0: return ')';
                    default: return keycode;
                }
            }
            return keycode;
        }
        
        // 处理特殊字符
        switch (keycode) {
            case SDLK_SPACE: return ' ';
            case SDLK_TAB: return '\t';
            case SDLK_RETURN: return '\r';
            case SDLK_ESCAPE: return 27;
            case SDLK_BACKSPACE: return '\b';
            
            // 标点符号
            case SDLK_SEMICOLON: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? ':' : ';';
            case SDLK_EQUALS: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '+' : '=';
            case SDLK_COMMA: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '<' : ',';
            case SDLK_MINUS: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '_' : '-';
            case SDLK_PERIOD: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '>' : '.';
            case SDLK_SLASH: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '?' : '/';
            case SDLK_GRAVE: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '~' : '`';
            case SDLK_LEFTBRACKET: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '{' : '[';
            case SDLK_BACKSLASH: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '|' : '\\';
            case SDLK_RIGHTBRACKET: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '}' : ']';
            case SDLK_APOSTROPHE: 
                return (modifiers & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT)) ? '"' : '\'';
                
            default:
                // 对于其他键，如果是可打印字符范围，直接返回
                if (keycode >= 32 && keycode <= 126) {
                    return keycode;
                }
                return 0; // 不可打印字符返回0
        }
    }
    
    int KeyboardEvent::getKeyLocationFromKeycode(SDL_Keycode keycode) {
        switch (keycode) {
            // 左侧修饰键
            case SDLK_LSHIFT:
            case SDLK_LCTRL:
            case SDLK_LALT:
            case SDLK_LGUI:
                return KEY_LOCATION_LEFT;
                
            // 右侧修饰键
            case SDLK_RSHIFT:
            case SDLK_RCTRL:
            case SDLK_RALT:
            case SDLK_RGUI:
                return KEY_LOCATION_RIGHT;
                
            // 数字键盘
            case SDLK_KP_0:
            case SDLK_KP_1:
            case SDLK_KP_2:
            case SDLK_KP_3:
            case SDLK_KP_4:
            case SDLK_KP_5:
            case SDLK_KP_6:
            case SDLK_KP_7:
            case SDLK_KP_8:
            case SDLK_KP_9:
            case SDLK_KP_PERIOD:
            case SDLK_KP_DIVIDE:
            case SDLK_KP_MULTIPLY:
            case SDLK_KP_MINUS:
            case SDLK_KP_PLUS:
            case SDLK_KP_ENTER:
            case SDLK_KP_EQUALS:
                return KEY_LOCATION_NUMPAD;
                
            default:
                return KEY_LOCATION_STANDARD;
        }
    }

} // namespace egret