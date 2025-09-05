//
// Created by wuxianggujun on 25-9-4.
//

#pragma once

#include <functional>
#include <string>

#include "core/HashObject.hpp"

namespace egret
{
    class Event;

    using EventListener = std::function<void(Event&)>;

    class IEventDispatcher : public virtual HashObject
    {
    public:
        ~IEventDispatcher() override =  default;

        virtual void addEventListener(const std::string& type,const EventListener& listener,void* thisObject = nullptr,
            bool useCapture = false,int priority = 0) = 0;

        virtual void once(const std::string& type,const EventListener& listener,void* thisObject = nullptr,bool useCapture = false,int priority = 0) = 0;

        virtual void removeEventListener(const std::string& type,const EventListener& listener,void* thisObject = nullptr,bool useCapture = false) = 0;

        virtual bool hasEventListener(const std::string& type) const = 0;

        virtual bool dispatchEvent(Event& event) = 0;

        virtual bool willTrigger(const std::string& type) const = 0;
        
        
    };
    
    
    
}
