//
// Created by wuxianggujun on 25-9-4.
//

#include "events/Event.hpp"
#include "events/IEventDispatcher.hpp"
#include "events/EventPhase.hpp"
#include <algorithm>

namespace egret {

    // ========== 静态成员定义 ==========
    
    // 事件类型常量定义
    const std::string Event::ADDED_TO_STAGE = "addedToStage";
    const std::string Event::REMOVED_FROM_STAGE = "removedFromStage";
    const std::string Event::ADDED = "added";
    const std::string Event::REMOVED = "removed";
    const std::string Event::ENTER_FRAME = "enterFrame";
    const std::string Event::RENDER = "render";
    const std::string Event::RESIZE = "resize";
    const std::string Event::CHANGE = "change";
    const std::string Event::CHANGING = "changing";
    const std::string Event::COMPLETE = "complete";
    const std::string Event::LOOP_COMPLETE = "loopComplete";
    const std::string Event::FOCUS_IN = "focusIn";
    const std::string Event::FOCUS_OUT = "focusOut";
    const std::string Event::ENDED = "ended";
    const std::string Event::ACTIVATE = "activate";
    const std::string Event::DEACTIVATE = "deactivate";
    const std::string Event::CLOSE = "close";
    const std::string Event::CONNECT = "connect";
    const std::string Event::LEAVE_STAGE = "leaveStage";
    const std::string Event::SOUND_COMPLETE = "soundComplete";
    
    // 静态成员变量定义
    std::vector<std::shared_ptr<Event>> Event::s_eventPool;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> Event::s_propertyData;
    
    // ========== 构造函数实现 ==========
    
    Event::Event(const std::string& type, bool bubbles, bool cancelable, const std::any& data)
        : HashObject()
        , m_type(type)
        , m_bubbles(bubbles)
        , m_cancelable(cancelable)
        , m_eventPhase(EventPhase::AT_TARGET)
        , m_currentTarget(nullptr)
        , m_target(nullptr)
        , m_isDefaultPrevented(false)
        , m_isPropagationStopped(false)
        , m_isPropagationImmediateStopped(false)
        , data(data)
    {
    }
    
    // ========== 事件控制方法实现 ==========
    
    void Event::preventDefault() {
        if (m_cancelable) {
            m_isDefaultPrevented = true;
        }
    }
    
    void Event::stopPropagation() {
        if (m_bubbles) {
            m_isPropagationStopped = true;
        }
    }
    
    void Event::stopImmediatePropagation() {
        if (m_bubbles) {
            m_isPropagationImmediateStopped = true;
        }
    }
    
    // ========== 内部方法实现 ==========
    
    bool Event::setTarget(const std::any& target) {
        m_target = target;
        return true;
    }
    
    // ========== 受保护的清理方法实现 ==========
    
    void Event::clean() {
        data = std::any();
        m_currentTarget = std::any();
        setTarget(std::any());
    }
    
    // ========== 静态工厂方法和对象池实现 ==========
    
    std::shared_ptr<Event> Event::create(const std::string& type, bool bubbles, bool cancelable) {
        // 尝试从对象池中获取可用的事件对象
        if (!s_eventPool.empty()) {
            auto event = s_eventPool.back();
            s_eventPool.pop_back();
            
            // 重置事件对象的状态
            event->m_type = type;
            event->m_bubbles = bubbles;
            event->m_cancelable = cancelable;
            event->m_isDefaultPrevented = false;
            event->m_isPropagationStopped = false;
            event->m_isPropagationImmediateStopped = false;
            event->m_eventPhase = EventPhase::AT_TARGET;
            event->m_currentTarget = std::any();
            event->m_target = std::any();
            event->data = std::any();
            
            return event;
        }
        
        // 如果对象池为空，创建新的事件对象
        return std::make_shared<Event>(type, bubbles, cancelable);
    }
    
    void Event::release(std::shared_ptr<Event> event) {
        if (!event) {
            return;
        }
        
        // 调用清理方法
        event->clean();
        
        // 将事件对象放回对象池
        s_eventPool.push_back(event);
    }
    
    bool Event::dispatchEvent(IEventDispatcher* target, const std::string& type, bool bubbles, const std::any& data) {
        if (!target) {
            return false;
        }
        
        // 从对象池创建事件对象
        auto event = Event::create(type, bubbles);
        
        // 设置属性数据
        auto& props = Event::getPropertyData("Event");
        if (data.has_value()) {
            props["data"] = data;
            event->data = data;
        }
        
        // 派发事件
        bool result = target->dispatchEvent(*event);
        
        // 释放事件对象回到池中
        Event::release(event);
        
        return result;
    }
    
    std::unordered_map<std::string, std::any>& Event::getPropertyData(const std::string& eventClass) {
        // 如果指定类的属性数据不存在，创建一个新的
        if (s_propertyData.find(eventClass) == s_propertyData.end()) {
            s_propertyData[eventClass] = std::unordered_map<std::string, std::any>();
        }
        return s_propertyData[eventClass];
    }

} // namespace egret
