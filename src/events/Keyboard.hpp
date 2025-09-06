#pragma once

#include <unordered_set>
#include <initializer_list>

namespace egret {

    /**
     * 通用键盘状态查询（全局键表）
     * - 由 SDLEventConverter 在 KEY_DOWN/KEY_UP 时更新
     * - 业务侧可在任意位置调用 getKeyboard().isDown(key) 查询是否按下
     */
    class Keyboard {
    public:
        static Keyboard& instance();

        // 更新某个键的按下/释放状态
        void setKeyDown(int keyCode, bool down);

        // 更新修饰键状态
        void setModifiers(bool ctrl, bool alt, bool shift);

        // 查询接口
        bool isDown(int keyCode) const;
        bool isAnyDown(std::initializer_list<int> keys) const;
        bool areAllDown(std::initializer_list<int> keys) const;

        bool isCtrlDown() const { return m_ctrl; }
        bool isAltDown()  const { return m_alt; }
        bool isShiftDown()const { return m_shift; }

    private:
        Keyboard() = default;
        std::unordered_set<int> m_down;
        bool m_ctrl = false;
        bool m_alt  = false;
        bool m_shift= false;
    };

    inline Keyboard& getKeyboard() { return Keyboard::instance(); }

} // namespace egret

