//
// Created by wuxianggujun on 25-9-4.
//

#include "TextEvent.hpp"
#include "IEventDispatcher.hpp"

namespace egret
{
    // ========== 静态常量定义 ==========
    const std::string TextEvent::LINK = "link";
    
    // ========== 静态成员变量定义 ==========
    std::vector<std::shared_ptr<TextEvent>> TextEvent::s_textEventPool;
    
    // ========== 构造函数 ==========
    TextEvent::TextEvent(const std::string& type, bool bubbles, bool cancelable, const std::string& text)
        : Event(type, bubbles, cancelable)
        , m_text(text)
    {
        // TextEvent的构造函数，调用父类Event的构造函数并初始化文本内容
    }
    
    // ========== 静态工厂方法实现 ==========
    std::shared_ptr<TextEvent> TextEvent::create(const std::string& type, bool bubbles, bool cancelable, const std::string& text)
    {
        std::shared_ptr<TextEvent> event;
        
        // 尝试从对象池中获取
        if (!s_textEventPool.empty()) {
            event = s_textEventPool.back();
            s_textEventPool.pop_back();
            
            // 重新设置属性
            event->setText(text);
            // 注意：由于Event的基础属性（type, bubbles等）是在构造时设置的
            // 在对象池复用时需要考虑这一点。这里暂时创建新对象
        }
        
        // 创建新实例
        if (!event) {
            event = std::shared_ptr<TextEvent>(new TextEvent(type, bubbles, cancelable, text));
        }
        
        return event;
    }
    
    void TextEvent::release(std::shared_ptr<TextEvent> event)
    {
        if (event) {
            // 清理事件状态
            event->clean();
            
            // 缓存到对象池（限制对象池大小）
            if (s_textEventPool.size() < 10) { // 限制对象池最大为10个对象
                s_textEventPool.push_back(event);
            }
            // 超出限制的对象将自动释放
        }
    }
    
    bool TextEvent::dispatchTextEvent(IEventDispatcher* target, const std::string& type, const std::string& text, bool bubbles)
    {
        if (!target) {
            return false;
        }
        
        // 创建TextEvent实例
        auto event = TextEvent::create(type, bubbles, false, text);
        
        // 派发事件
        bool result = target->dispatchEvent(*event);
        
        // 释放事件对象到对象池
        TextEvent::release(event);
        
        return result;
    }
    
    // ========== 受保护的清理方法 ==========
    void TextEvent::clean()
    {
        // 调用父类的清理方法
        Event::clean();
        
        // 清理TextEvent特有的属性
        m_text.clear();
    }
    
} // namespace egret