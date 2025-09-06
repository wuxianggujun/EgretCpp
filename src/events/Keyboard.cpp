#include "events/Keyboard.hpp"

namespace egret {

Keyboard& Keyboard::instance() {
    static Keyboard inst;
    return inst;
}

void Keyboard::setKeyDown(int keyCode, bool down) {
    if (down) {
        m_down.insert(keyCode);
    } else {
        m_down.erase(keyCode);
    }
}

void Keyboard::setModifiers(bool ctrl, bool alt, bool shift) {
    m_ctrl = ctrl;
    m_alt = alt;
    m_shift = shift;
}

bool Keyboard::isDown(int keyCode) const {
    return m_down.find(keyCode) != m_down.end();
}

bool Keyboard::isAnyDown(std::initializer_list<int> keys) const {
    for (int k : keys) {
        if (m_down.find(k) != m_down.end()) return true;
    }
    return false;
}

bool Keyboard::areAllDown(std::initializer_list<int> keys) const {
    for (int k : keys) {
        if (m_down.find(k) == m_down.end()) return false;
    }
    return true;
}

} // namespace egret

