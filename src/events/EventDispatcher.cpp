//
// Created by wuxianggujun on 25-9-4.
//

#include "EventDispatcher.hpp"

#include "Event.hpp"

namespace egret
{
    std::vector<EventBin> EventDispatcher::s_onceEventList;


    EventDispatcher::EventDispatcher(IEventDispatcher* target)
    {
        m_eventDispatcher.eventTarget = target ? target : this;
        m_eventDispatcher.notifyLevel = 0;
    }

    void EventDispatcher::addEventListener(const std::string& type, const EventListener& listener, void* thisObject,
                                           bool useCapture, int priority)
    {
        addListener(type, listener, thisObject, useCapture, priority, false);
    }

    void EventDispatcher::once(const std::string& type, const EventListener& listener, void* thisObject,
                               bool useCapture, int priority)
    {
        addListener(type, listener, thisObject, useCapture, priority, true);
    }

    void EventDispatcher::removeEventListener(const std::string& type, const EventListener& listener, void* thisObject,
                                              bool useCapture)
    {
        auto& eventMap = getEventMap(useCapture);
        auto it = eventMap.find(type);
        if (it == eventMap.end())
        {
            return;
        }

        auto& list = it->second;
        if (m_eventDispatcher.notifyLevel != 0)
        {
            // 正在派发事件时，复制列表以避免修改原始列表
            eventMap[type] = list; // 这里会触发拷贝
        }

        if (removeEventBin(list, listener, thisObject))
        {
            if (list.empty())
            {
                eventMap.erase(type);
            }
        }
    }

    bool EventDispatcher::hasEventListener(const std::string& type) const
    {
        auto& eventMap = const_cast<EventDispatcher*>(this)->getEventMap(false);
        auto& captureEventsMap = const_cast<EventDispatcher*>(this)->getEventMap(true);
        return eventMap.find(type) != eventMap.end() || captureEventsMap.find(type) != captureEventsMap.end();
    }

    bool EventDispatcher::dispatchEvent(Event& event)
    {
        event.setCurrentTarget(m_eventDispatcher.eventTarget);
        event.setTarget(m_eventDispatcher.eventTarget);
        return notifyListener(event, false);
    }

    bool EventDispatcher::willTrigger(const std::string& type) const
    {
        return hasEventListener(type);
    }

    bool EventDispatcher::dispatchEventWith(const std::string& type, bool bubbles, void* data, bool cancelable)
    {
        if (bubbles || hasEventListener(type))
        {
            auto event = Event::create(type, bubbles, cancelable);
            event->data = std::move(data);
            bool result = dispatchEvent(*event);
            Event::release(event);
            return result;
        }
        return true;
    }

    std::map<std::string, std::vector<EventBin>>& EventDispatcher::getEventMap(bool useCapture)
    {
        return useCapture ? m_eventDispatcher.captureEventsMap : m_eventDispatcher.eventsMap;
    }

    void EventDispatcher::addListener(const std::string& type, const EventListener& listener, void* thisObject,
                                      bool useCapture, int priority, bool dispatchOnce)
    {
        auto& eventMap = getEventMap(useCapture);
        auto& list = eventMap[type];

        if (m_eventDispatcher.notifyLevel != 0)
        {
            list = eventMap[type];
        }

        insertEventBin(list, type, listener, thisObject, useCapture, priority, dispatchOnce);
    }

    bool EventDispatcher::insertEventBin(std::vector<EventBin>& list, const std::string& type,
                                         const EventListener& listener, void* thisObject, bool useCapture, int priority,
                                         bool dispatchOnce)
    {
        // 检查是否已存在相同的监听器
        for (const auto& bin : list)
        {
            if (bin.thisObject == thisObject && bin.target == this)
            {
                // TODO: 注意：这里无法直接比较std::function，简化处理
                // 在实际使用中可能需要其他方式来标识唯一监听器
                return false;
            }
        }

        const EventBin eventBin{type, listener, thisObject, priority, this, useCapture, dispatchOnce};

        auto insertPos = std::find_if(list.begin(), list.end(),
                                      [priority](const EventBin& bin)
                                      {
                                          return bin.priority == priority;
                                      });
        list.insert(insertPos, eventBin);
        return true;
    }

    bool EventDispatcher::removeEventBin(std::vector<EventBin>& list, const EventListener& listener, void* thisObject)
    {
        auto it = std::find_if(list.begin(), list.end(), [thisObject,this](const EventBin& bin)
        {
            return bin.thisObject == thisObject && bin.target == this;
        });

        if (it != list.end())
        {
            list.erase(it);
            return true;
        }
        return false;
    }

    bool EventDispatcher::notifyListener(Event& event, bool capturePhase)
    {
        auto& eventMap = getEventMap(capturePhase);
        auto it = eventMap.find(event.getType());
        if (it == eventMap.end())
        {
            return true;
        }

        const auto& list = it->second;
        if (list.empty())
        {
            return true;
        }

        m_eventDispatcher.notifyLevel++;

        for (auto& eventBin : list)
        {
            eventBin.listener(event);

            if (eventBin.dispatchOnce)
            {
                s_onceEventList.push_back(eventBin);
            }

            if (event.isPropagationImmediateStopped())
            {
                break;
            }
        }

        m_eventDispatcher.notifyLevel--;

        while (!s_onceEventList.empty())
        {
            auto& eventBin = s_onceEventList.back();
            eventBin.target->removeEventListener(eventBin.type, eventBin.listener, eventBin.thisObject,
                                                 eventBin.useCapture);
            s_onceEventList.pop_back();
        }

        return !event.isDefaultPrevented();
    }
} // egret
