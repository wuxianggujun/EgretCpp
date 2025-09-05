//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include "events/Event.hpp"
#include <string>
#include <memory>

namespace egret
{
    /**
     * 每当Timer对象达到由Timer.delay属性指定的间隔时，Timer对象即会调度TimerEvent对象。
     * TimerEvent类继承自Event类，提供定时器相关的事件处理。
     * @version Egret 2.4
     * @platform Web,Native
     */
    class TimerEvent : public Event
    {
    public:
        // ========== 事件类型常量 ==========
        
        /**
         * 每当Timer对象达到根据Timer.delay属性指定的间隔时调度
         * @version Egret 2.4
         * @platform Web,Native
         */
        static const std::string TIMER;
        
        /**
         * 每当它完成Timer.repeatCount设置的请求数后调度
         * @version Egret 2.4
         * @platform Web,Native
         */
        static const std::string TIMER_COMPLETE;
        
        // ========== 构造函数 ==========
        
        /**
         * 创建一个Event对象，其中包含有关timer事件的特定信息
         * @param type 事件的类型。事件侦听器可以通过继承的type属性访问此信息
         * @param bubbles 确定Event对象是否冒泡。事件侦听器可以通过继承的bubbles属性访问此信息
         * @param cancelable 确定是否可以取消Event对象。事件侦听器可以通过继承的cancelable属性访问此信息
         * @version Egret 2.4
         * @platform Web,Native
         */
        explicit TimerEvent(const std::string& type, bool bubbles = false, bool cancelable = false);
        
        /**
         * 虚析构函数
         */
        virtual ~TimerEvent() = default;
        
        // ========== 静态工厂方法 ==========
        
        /**
         * 从对象池中取出或创建一个新的TimerEvent实例
         * @param type 事件的类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @param cancelable 确定是否可以取消Event对象
         * @return TimerEvent实例
         */
        static std::shared_ptr<TimerEvent> create(const std::string& type, bool bubbles = false, bool cancelable = false);
        
        /**
         * 释放一个TimerEvent对象，并缓存到对象池
         * @param event 要释放的事件对象（必须是由TimerEvent::create()创建的）
         */
        static void release(std::shared_ptr<TimerEvent> event);
        
        /**
         * 派发定时器事件的便捷方法
         * @param target 派发事件目标
         * @param type 事件类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @return 事件是否成功派发
         */
        static bool dispatchTimerEvent(IEventDispatcher* target, const std::string& type, bool bubbles = false);
        
    protected:
        /**
         * 清理方法，用于对象池复用时重置状态
         */
        virtual void clean() override;
        
    private:
        /** TimerEvent对象池 */
        static std::vector<std::shared_ptr<TimerEvent>> s_timerEventPool;
    };
    
} // namespace egret