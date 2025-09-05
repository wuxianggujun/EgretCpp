//
// Created by wuxianggujun on 25-9-4.
//

#include "ProgressEvent.hpp"
#include "IEventDispatcher.hpp"

namespace egret
{
    // ========== 静态常量定义 ==========
    const std::string ProgressEvent::PROGRESS = "progress";
    const std::string ProgressEvent::SOCKET_DATA = "socketData";
    
    // ========== 静态成员变量定义 ==========
    std::vector<std::shared_ptr<ProgressEvent>> ProgressEvent::s_progressEventPool;
    
    // ========== 构造函数 ==========
    ProgressEvent::ProgressEvent(const std::string& type, bool bubbles, bool cancelable, double bytesLoaded, double bytesTotal)
        : Event(type, bubbles, cancelable)
        , m_bytesLoaded(bytesLoaded)
        , m_bytesTotal(bytesTotal)
    {
        // ProgressEvent的构造函数，调用父类Event的构造函数并初始化进度信息
    }
    
    // ========== 静态工厂方法实现 ==========
    std::shared_ptr<ProgressEvent> ProgressEvent::create(const std::string& type, bool bubbles, bool cancelable, double bytesLoaded, double bytesTotal)
    {
        std::shared_ptr<ProgressEvent> event;
        
        // 尝试从对象池中获取
        if (!s_progressEventPool.empty()) {
            event = s_progressEventPool.back();
            s_progressEventPool.pop_back();
            
            // 重新设置属性
            event->setBytesLoaded(bytesLoaded);
            event->setBytesTotal(bytesTotal);
        }
        
        // 创建新实例
        if (!event) {
            event = std::shared_ptr<ProgressEvent>(new ProgressEvent(type, bubbles, cancelable, bytesLoaded, bytesTotal));
        }
        
        return event;
    }
    
    void ProgressEvent::release(std::shared_ptr<ProgressEvent> event)
    {
        if (event) {
            // 清理事件状态
            event->clean();
            
            // 缓存到对象池（限制对象池大小）
            if (s_progressEventPool.size() < 10) { // 限制对象池最大为10个对象
                s_progressEventPool.push_back(event);
            }
            // 超出限制的对象将自动释放
        }
    }
    
    bool ProgressEvent::dispatchProgressEvent(IEventDispatcher* target, const std::string& type, double bytesLoaded, double bytesTotal, bool bubbles)
    {
        if (!target) {
            return false;
        }
        
        // 创建ProgressEvent实例
        auto event = ProgressEvent::create(type, bubbles, false, bytesLoaded, bytesTotal);
        
        // 派发事件
        bool result = target->dispatchEvent(*event);
        
        // 释放事件对象到对象池
        ProgressEvent::release(event);
        
        return result;
    }
    
    // ========== 受保护的清理方法 ==========
    void ProgressEvent::clean()
    {
        // 调用父类的清理方法
        Event::clean();
        
        // 清理ProgressEvent特有的属性
        m_bytesLoaded = 0.0;
        m_bytesTotal = 0.0;
    }
    
} // namespace egret