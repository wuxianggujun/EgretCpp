//
// Created by wuxianggujun on 25-9-4.
//

#include "EventDispatcher.hpp"

#include "Event.hpp"
#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"

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
        // 设置事件目标（首次）
        event.setTarget(m_eventDispatcher.eventTarget);

        // 如果目标是DisplayObject，构建祖先链路以支持 捕获->目标->冒泡
        DisplayObject* targetDO = nullptr;
        try {
            targetDO = std::any_cast<DisplayObject*>(event.getTarget());
        } catch (...) {
            targetDO = nullptr;
        }

        if (!targetDO) {
            // 非显示对象，直接在当前dispatcher上触发
            event.setCurrentTarget(m_eventDispatcher.eventTarget);
            event.setEventPhase(EventPhase::AT_TARGET);
            return notifyListener(event, false);
        }

        // 构建从Stage到目标的祖先链（不含空）
        std::vector<DisplayObject*> chain;
        DisplayObject* node = targetDO;
        while (node) {
            chain.push_back(node);
            node = node->getParent();
        }
        // chain: [target, parent, ..., stage]，我们需要从后往前捕获

        // 捕获阶段：stage -> parent（不包括 target）
        for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
            DisplayObject* current = *it;
            if (current == targetDO) break;
            event.setCurrentTarget(current);
            event.setEventPhase(EventPhase::CAPTURING_PHASE);
            notifyListener(event, true);
            if (event.isPropagationStopped() || event.isPropagationImmediateStopped()) {
                return !event.isDefaultPrevented();
            }
        }

        // 目标阶段：在 target 上先触发捕获监听（useCapture=true），再触发冒泡监听（useCapture=false）
        event.setCurrentTarget(targetDO);
        event.setEventPhase(EventPhase::AT_TARGET);
        notifyListener(event, true);
        if (!(event.isPropagationStopped() || event.isPropagationImmediateStopped())) {
            notifyListener(event, false);
        }
        if (event.isPropagationStopped() || event.isPropagationImmediateStopped()) {
            return !event.isDefaultPrevented();
        }

        // 冒泡阶段：从 parent -> stage（仅当事件可冒泡）
        if (event.getBubbles()) {
            for (size_t i = 1; i < chain.size(); ++i) {
                DisplayObject* current = chain[i];
                event.setCurrentTarget(current);
                event.setEventPhase(EventPhase::BUBBLING_PHASE);
                notifyListener(event, false);
                if (event.isPropagationStopped() || event.isPropagationImmediateStopped()) {
                    break;
                }
            }
        }

        return !event.isDefaultPrevented();
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

        // 正在派发期间，为该类型的监听列表做一次性拷贝，避免边派发边修改产生迭代风险
        if (m_eventDispatcher.notifyLevel != 0)
        {
            auto& clonedSet = useCapture ? m_eventDispatcher.clonedCaptureTypes : m_eventDispatcher.clonedTypes;
            if (clonedSet.find(type) == clonedSet.end()) {
                auto copy = list; // 拷贝一份
                list = std::move(copy);
                clonedSet.insert(type);
            }
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

        if (m_eventDispatcher.notifyLevel == 0) {
            // 一次派发结束，清理克隆标记
            m_eventDispatcher.clonedTypes.clear();
            m_eventDispatcher.clonedCaptureTypes.clear();
        }

        return !event.isDefaultPrevented();
    }
} // egret
