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
     * 当加载操作已开始或套接字已接收到数据时，将调度ProgressEvent对象。
     * 有两种类型的进程事件：ProgressEvent::PROGRESS和ProgressEvent::SOCKET_DATA。
     * ProgressEvent类继承自Event类，提供进度相关的事件处理。
     * @version Egret 2.4
     * @platform Web,Native
     */
    class ProgressEvent : public Event
    {
    public:
        // ========== 事件类型常量 ==========
        
        /**
         * 加载进度发生变化
         * @version Egret 2.4
         * @platform Web,Native
         */
        static const std::string PROGRESS;
        
        /**
         * 获取到数据
         * @version Egret 2.4
         * @platform Web,Native
         */
        static const std::string SOCKET_DATA;
        
        // ========== 构造函数 ==========
        
        /**
         * 创建一个ProgressEvent对象
         * @param type 事件的类型，可以作为Event::type访问
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段。默认值为false
         * @param cancelable 确定是否可以取消Event对象。默认值为false
         * @param bytesLoaded 在侦听器处理事件时加载的项数或字节数
         * @param bytesTotal 如果加载过程成功，将加载的总项数或总字节数
         * @version Egret 2.4
         * @platform Web,Native
         */
        explicit ProgressEvent(const std::string& type, bool bubbles = false, bool cancelable = false, 
                              double bytesLoaded = 0.0, double bytesTotal = 0.0);
        
        /**
         * 虚析构函数
         */
        virtual ~ProgressEvent() = default;
        
        // ========== 公开属性访问器 ==========
        
        /**
         * 在侦听器处理事件时加载的项数或字节数
         * @version Egret 2.4
         * @platform Web,Native
         */
        double getBytesLoaded() const { return m_bytesLoaded; }
        
        /**
         * 设置已加载字节数（内部使用）
         * @param bytesLoaded 已加载字节数
         */
        void setBytesLoaded(double bytesLoaded) { m_bytesLoaded = bytesLoaded; }
        
        /**
         * 如果加载过程成功，将加载的总项数或总字节数
         * @version Egret 2.4
         * @platform Web,Native
         */
        double getBytesTotal() const { return m_bytesTotal; }
        
        /**
         * 设置总字节数（内部使用）
         * @param bytesTotal 总字节数
         */
        void setBytesTotal(double bytesTotal) { m_bytesTotal = bytesTotal; }
        
        // ========== 静态工厂方法 ==========
        
        /**
         * 从对象池中取出或创建一个新的ProgressEvent实例
         * @param type 事件的类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @param cancelable 确定是否可以取消Event对象
         * @param bytesLoaded 已加载字节数
         * @param bytesTotal 总字节数
         * @return ProgressEvent实例
         */
        static std::shared_ptr<ProgressEvent> create(const std::string& type, bool bubbles = false, bool cancelable = false, 
                                                    double bytesLoaded = 0.0, double bytesTotal = 0.0);
        
        /**
         * 释放一个ProgressEvent对象，并缓存到对象池
         * @param event 要释放的事件对象（必须是由ProgressEvent::create()创建的）
         */
        static void release(std::shared_ptr<ProgressEvent> event);
        
        /**
         * 派发进度事件的便捷方法
         * @param target 派发事件目标
         * @param type 事件类型
         * @param bytesLoaded 已加载字节数
         * @param bytesTotal 总字节数
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @return 事件是否成功派发
         */
        static bool dispatchProgressEvent(IEventDispatcher* target, const std::string& type, 
                                         double bytesLoaded = 0.0, double bytesTotal = 0.0, bool bubbles = false);
        
    protected:
        /**
         * 清理方法，用于对象池复用时重置状态
         */
        virtual void clean() override;
        
    private:
        /**
         * 在侦听器处理事件时加载的项数或字节数
         */
        double m_bytesLoaded;
        
        /**
         * 如果加载过程成功，将加载的总项数或总字节数
         */
        double m_bytesTotal;
        
        /** ProgressEvent对象池 */
        static std::vector<std::shared_ptr<ProgressEvent>> s_progressEventPool;
    };
    
} // namespace egret