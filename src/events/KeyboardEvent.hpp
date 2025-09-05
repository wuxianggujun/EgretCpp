#pragma once

#include "events/Event.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <memory>

namespace egret {

    class IEventDispatcher;

    /**
     * KeyboardEvent 类用于处理键盘输入事件
     * 集成 SDL3 键盘事件系统，为 C++ 版本提供完整的键盘输入支持
     */
    class KeyboardEvent : public Event {
    public:
        // ========== 事件类型常量 ==========
        
        /** 按下键盘按键时调度 */
        static const std::string KEY_DOWN;
        /** 释放键盘按键时调度 */
        static const std::string KEY_UP;

        // ========== 键盘位置常量 ==========
        
        /** 标准键盘位置 */
        static const int KEY_LOCATION_STANDARD = 0;
        /** 左侧修饰键位置（如左Shift、左Ctrl） */
        static const int KEY_LOCATION_LEFT = 1;
        /** 右侧修饰键位置（如右Shift、右Ctrl） */
        static const int KEY_LOCATION_RIGHT = 2;
        /** 数字键盘位置 */
        static const int KEY_LOCATION_NUMPAD = 3;

        // ========== 构造函数 ==========
        
        /**
         * 创建 KeyboardEvent 实例
         * @param type 事件类型（KEY_DOWN 或 KEY_UP）
         * @param bubbles 是否冒泡，默认 true
         * @param cancelable 是否可取消，默认 true
         * @param keyCode 键码值
         * @param charCode 字符码值
         * @param keyLocation 按键位置
         * @param ctrlKey 是否按下 Ctrl 键
         * @param altKey 是否按下 Alt 键
         * @param shiftKey 是否按下 Shift 键
         */
        explicit KeyboardEvent(const std::string& type, bool bubbles = true, bool cancelable = true,
                              int keyCode = 0, int charCode = 0, int keyLocation = KEY_LOCATION_STANDARD,
                              bool ctrlKey = false, bool altKey = false, bool shiftKey = false);

        virtual ~KeyboardEvent() = default;

        // ========== 属性访问器 ==========
        
        /**
         * 获取键码值
         * 对应 SDL_Keycode，表示物理按键
         */
        int getKeyCode() const { return m_keyCode; }
        
        /**
         * 设置键码值（内部使用）
         */
        void setKeyCode(int keyCode) { m_keyCode = keyCode; }
        
        /**
         * 获取字符码值
         * 表示按键产生的字符的 Unicode 值
         */
        int getCharCode() const { return m_charCode; }
        
        /**
         * 设置字符码值（内部使用）
         */
        void setCharCode(int charCode) { m_charCode = charCode; }
        
        /**
         * 获取按键位置
         * 区分左右修饰键和数字键盘
         */
        int getKeyLocation() const { return m_keyLocation; }
        
        /**
         * 设置按键位置（内部使用）
         */
        void setKeyLocation(int keyLocation) { m_keyLocation = keyLocation; }
        
        /**
         * 是否按下了 Ctrl 键
         */
        bool getCtrlKey() const { return m_ctrlKey; }
        
        /**
         * 设置 Ctrl 键状态（内部使用）
         */
        void setCtrlKey(bool ctrlKey) { m_ctrlKey = ctrlKey; }
        
        /**
         * 是否按下了 Alt 键
         */
        bool getAltKey() const { return m_altKey; }
        
        /**
         * 设置 Alt 键状态（内部使用）
         */
        void setAltKey(bool altKey) { m_altKey = altKey; }
        
        /**
         * 是否按下了 Shift 键
         */
        bool getShiftKey() const { return m_shiftKey; }
        
        /**
         * 设置 Shift 键状态（内部使用）
         */
        void setShiftKey(bool shiftKey) { m_shiftKey = shiftKey; }

        // ========== SDL3 集成方法 ==========
        
        /**
         * 从 SDL 键盘事件初始化
         * @param sdlEvent SDL 键盘事件
         */
        void initFromSDLEvent(const SDL_Event& sdlEvent);
        
        /**
         * 获取按键名称
         * 返回人类可读的按键名称
         */
        std::string getKeyName() const;

        // ========== 静态工厂方法 ==========
        
        /**
         * 从对象池中创建 KeyboardEvent 实例
         * @param type 事件类型
         * @param keyCode 键码值
         * @param charCode 字符码值
         * @param keyLocation 按键位置
         * @param ctrlKey 是否按下 Ctrl 键
         * @param altKey 是否按下 Alt 键
         * @param shiftKey 是否按下 Shift 键
         * @return KeyboardEvent 实例
         */
        static std::shared_ptr<KeyboardEvent> create(const std::string& type, 
                                                   int keyCode = 0, int charCode = 0, 
                                                   int keyLocation = KEY_LOCATION_STANDARD,
                                                   bool ctrlKey = false, bool altKey = false, 
                                                   bool shiftKey = false);
        
        /**
         * 从 SDL 事件创建 KeyboardEvent
         * @param sdlEvent SDL 键盘事件
         * @return KeyboardEvent 实例
         */
        static std::shared_ptr<KeyboardEvent> createFromSDLEvent(const SDL_Event& sdlEvent);
        
        /**
         * 使用指定的 EventDispatcher 派发键盘事件
         * @param target 事件目标
         * @param type 事件类型
         * @param keyCode 键码值
         * @param charCode 字符码值
         * @param keyLocation 按键位置
         * @param ctrlKey 是否按下 Ctrl 键
         * @param altKey 是否按下 Alt 键
         * @param shiftKey 是否按下 Shift 键
         * @return 是否成功派发
         */
        static bool dispatchKeyboardEvent(IEventDispatcher* target, const std::string& type,
                                        int keyCode = 0, int charCode = 0, 
                                        int keyLocation = KEY_LOCATION_STANDARD,
                                        bool ctrlKey = false, bool altKey = false, 
                                        bool shiftKey = false);

    protected:
        /**
         * 清理方法，用于对象池复用
         */
        void clean() override;

    private:
        // ========== 私有成员变量 ==========
        
        int m_keyCode;          // 键码值（SDL_Keycode）
        int m_charCode;         // 字符码值（Unicode）
        int m_keyLocation;      // 按键位置
        bool m_ctrlKey;         // Ctrl 键状态
        bool m_altKey;          // Alt 键状态
        bool m_shiftKey;        // Shift 键状态

        // ========== 静态辅助方法 ==========
        
        /**
         * 将 SDL 键码转换为字符码
         * @param keycode SDL 键码
         * @param modifiers 修饰键状态
         * @return 字符码值
         */
        static int convertKeyCodeToCharCode(SDL_Keycode keycode, SDL_Keymod modifiers);
        
        /**
         * 获取键盘位置信息
         * @param keycode SDL 键码
         * @return 键盘位置常量
         */
        static int getKeyLocationFromKeycode(SDL_Keycode keycode);
    };

} // namespace egret