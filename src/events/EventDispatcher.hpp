//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include <map>
#include <string>
#include <unordered_set>

#include "events/IEventDispatcher.hpp"


namespace egret
{
    struct EventBin
    {
        std::string type;
        EventListener listener;
        void* thisObject;
        int priority;
        IEventDispatcher* target;
        bool useCapture;
        bool dispatchOnce;
    };

    class EventDispatcher : public virtual IEventDispatcher
    {
    private:
        struct EventDispatcherData
        {
            IEventDispatcher* eventTarget;
            std::map<std::string, std::vector<EventBin>> eventsMap;
            std::map<std::string, std::vector<EventBin>> captureEventsMap;
            int notifyLevel;
            // 在一次派发期间，哪些 type 已经克隆过，避免重复复制
            std::unordered_set<std::string> clonedTypes;
            std::unordered_set<std::string> clonedCaptureTypes;
        };

        EventDispatcherData m_eventDispatcher;
       static std::vector<EventBin> s_onceEventList;

    public:
        explicit EventDispatcher(IEventDispatcher* target = nullptr);
        ~EventDispatcher() override = default;

        void addEventListener(const std::string& type, const EventListener& listener, void* thisObject, bool useCapture,
                              int priority) override;

        void once(const std::string& type, const EventListener& listener, void* thisObject, bool useCapture,
                  int priority) override;

        void removeEventListener(const std::string& type, const EventListener& listener, void* thisObject,
                                 bool useCapture) override;

        bool hasEventListener(const std::string& type) const override;
        bool dispatchEvent(Event& event) override;
        bool willTrigger(const std::string& type) const override;

        bool dispatchEventWith(const std::string& type, bool bubbles = false, void* data = nullptr,
                               bool cancelable = false);

    private:
        std::map<std::string, std::vector<EventBin>>& getEventMap(bool useCapture = false);

        void addListener(const std::string& type,
                         const EventListener& listener,
                         void* thisObject,
                         bool useCapture,
                         int priority,
                         bool dispatchOnce = false);

        bool insertEventBin(std::vector<EventBin>& list,
                            const std::string& type,
                            const EventListener& listener,
                            void* thisObject,
                            bool useCapture,
                            int priority,
                            bool dispatchOnce);

        bool removeEventBin(std::vector<EventBin>& list,
                            const EventListener& listener,
                            void* thisObject);

        bool notifyListener(Event& event, bool capturePhase);
    };
} // egret
