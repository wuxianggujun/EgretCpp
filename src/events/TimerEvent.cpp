//
// Created by wuxianggujun on 25-9-4.
//

#include "TimerEvent.hpp"
#include "IEventDispatcher.hpp"

namespace egret
{
    // ========== 静态常量定义 ==========
    const std::string TimerEvent::TIMER = "timer";
    const std::string TimerEvent::TIMER_COMPLETE = "timerComplete";
    
    // ========== 静态成员变量定义 ==========
    std::vector<std::shared_ptr<TimerEvent>> TimerEvent::s_timerEventPool;
    
    // ========== 构造函数 ==========
    TimerEvent::TimerEvent(const std::string& type, bool bubbles, bool cancelable)
        : Event(type, bubbles, cancelable)
    {
        // TimerEvent的构造函数，调用父类Event的构造函数
    }
    
    // ========== 静态工厂方法实现 ==========
    std::shared_ptr<TimerEvent> TimerEvent::create(const std::string& type, bool bubbles, bool cancelable)
    {
        std::shared_ptr<TimerEvent> event;
        
        // 尝试从对象池中获取
        if (!s_timerEventPool.empty()) {
            event = s_timerEventPool.back();
            s_timerEventPool.pop_back();
            
            // 重新初始化事件对象
            // 由于Event的成员变量是private，我们需要通过构造函数重新初始化
            // 这里我们直接创建新对象，因为重用机制需要更复杂的设计
        }
        
        // 创建新实例
        if (!event) {
            event = std::shared_ptr<TimerEvent>(new TimerEvent(type, bubbles, cancelable));
        }
        
        return event;
    }
    
    void TimerEvent::release(std::shared_ptr<TimerEvent> event)
    {
        if (event) {
            // 清理事件状态
            event->clean();
            
            // 缓存到对象池（限制对象池大小）
            if (s_timerEventPool.size() < 10) { // 限制对象池最大为10个对象
                s_timerEventPool.push_back(event);
            }
            // 超出限制的对象将自动释放
        }
    }
    
    bool TimerEvent::dispatchTimerEvent(IEventDispatcher* target, const std::string& type, bool bubbles)
    {
        if (!target) {
            return false;
        }
        
        // 创建TimerEvent实例
        auto event = TimerEvent::create(type, bubbles, false);
        
        // 派发事件
        bool result = target->dispatchEvent(*event);
        
        // 释放事件对象到对象池
        TimerEvent::release(event);
        
        return result;
    }
    
    // ========== 受保护的清理方法 ==========
    void TimerEvent::clean()
    {
        // 调用父类的清理方法
        Event::clean();
        
        // TimerEvent特有的清理逻辑（当前没有特殊属性需要清理）
        // 如果将来添加了TimerEvent特有的属性，在这里进行清理
    }
    
} // namespace egret