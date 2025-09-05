//
// Created by wuxianggujun on 25-9-4.
//

#include "IOErrorEvent.hpp"
#include "IEventDispatcher.hpp"

namespace egret
{
    // ========== 静态常量定义 ==========
    const std::string IOErrorEvent::IO_ERROR = "ioError";
    
    // ========== 静态成员变量定义 ==========
    std::vector<std::shared_ptr<IOErrorEvent>> IOErrorEvent::s_ioErrorEventPool;
    
    // ========== 构造函数 ==========
    IOErrorEvent::IOErrorEvent(const std::string& type, bool bubbles, bool cancelable)
        : Event(type, bubbles, cancelable)
    {
        // IOErrorEvent的构造函数，调用父类Event的构造函数
    }
    
    // ========== 静态工厂方法实现 ==========
    std::shared_ptr<IOErrorEvent> IOErrorEvent::create(const std::string& type, bool bubbles, bool cancelable)
    {
        std::shared_ptr<IOErrorEvent> event;
        
        // 尝试从对象池中获取
        if (!s_ioErrorEventPool.empty()) {
            event = s_ioErrorEventPool.back();
            s_ioErrorEventPool.pop_back();
        }
        
        // 创建新实例
        if (!event) {
            event = std::shared_ptr<IOErrorEvent>(new IOErrorEvent(type, bubbles, cancelable));
        }
        
        return event;
    }
    
    void IOErrorEvent::release(std::shared_ptr<IOErrorEvent> event)
    {
        if (event) {
            // 清理事件状态
            event->clean();
            
            // 缓存到对象池（限制对象池大小）
            if (s_ioErrorEventPool.size() < 10) { // 限制对象池最大为10个对象
                s_ioErrorEventPool.push_back(event);
            }
            // 超出限制的对象将自动释放
        }
    }
    
    bool IOErrorEvent::dispatchIOErrorEvent(IEventDispatcher* target, const std::string& type, bool bubbles)
    {
        if (!target) {
            return false;
        }
        
        // 创建IOErrorEvent实例
        auto event = IOErrorEvent::create(type, bubbles, false);
        
        // 派发事件
        bool result = target->dispatchEvent(*event);
        
        // 释放事件对象到对象池
        IOErrorEvent::release(event);
        
        return result;
    }
    
    // ========== 受保护的清理方法 ==========
    void IOErrorEvent::clean()
    {
        // 调用父类的清理方法
        Event::clean();
        
        // IOErrorEvent特有的清理逻辑（当前没有特殊属性需要清理）
    }
    
} // namespace egret