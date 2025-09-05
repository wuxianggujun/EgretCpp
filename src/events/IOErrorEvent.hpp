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
     * IO流事件，当错误导致输入或输出操作失败时调度IOErrorEvent对象。
     * IOErrorEvent类继承自Event类，提供IO错误相关的事件处理。
     * @version Egret 2.4
     * @platform Web,Native
     */
    class IOErrorEvent : public Event
    {
    public:
        // ========== 事件类型常量 ==========
        
        /**
         * io发生错误
         * @version Egret 2.4
         * @platform Web,Native
         */
        static const std::string IO_ERROR;
        
        // ========== 构造函数 ==========
        
        /**
         * 创建一个IOErrorEvent对象
         * @param type 事件的类型，可以作为Event::type访问
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段。默认值为false
         * @param cancelable 确定是否可以取消Event对象。默认值为false
         * @version Egret 2.4
         * @platform Web,Native
         */
        explicit IOErrorEvent(const std::string& type, bool bubbles = false, bool cancelable = false);
        
        /**
         * 虚析构函数
         */
        virtual ~IOErrorEvent() = default;
        
        // ========== 静态工厂方法 ==========
        
        /**
         * 从对象池中取出或创建一个新的IOErrorEvent实例
         * @param type 事件的类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @param cancelable 确定是否可以取消Event对象
         * @return IOErrorEvent实例
         */
        static std::shared_ptr<IOErrorEvent> create(const std::string& type, bool bubbles = false, bool cancelable = false);
        
        /**
         * 释放一个IOErrorEvent对象，并缓存到对象池
         * @param event 要释放的事件对象（必须是由IOErrorEvent::create()创建的）
         */
        static void release(std::shared_ptr<IOErrorEvent> event);
        
        /**
         * 派发IO错误事件的便捷方法
         * @param target 派发事件目标
         * @param type 事件类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @return 事件是否成功派发
         */
        static bool dispatchIOErrorEvent(IEventDispatcher* target, const std::string& type, bool bubbles = false);
        
    protected:
        /**
         * 清理方法，用于对象池复用时重置状态
         */
        virtual void clean() override;
        
    private:
        /** IOErrorEvent对象池 */
        static std::vector<std::shared_ptr<IOErrorEvent>> s_ioErrorEventPool;
    };
    
} // namespace egret