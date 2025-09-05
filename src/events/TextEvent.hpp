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
     * 用户在富文本中单击超链接时，对象将调度TextEvent对象。文本事件类型：TextEvent::LINK。
     * TextEvent类继承自Event类，提供文本相关的事件处理。
     * @version Egret 2.4
     * @platform Web,Native
     */
    class TextEvent : public Event
    {
    public:
        // ========== 事件类型常量 ==========
        
        /**
         * 定义link事件对象的type属性值
         * @version Egret 2.4
         * @platform Web,Native
         */
        static const std::string LINK;
        
        // ========== 构造函数 ==========
        
        /**
         * 创建一个TextEvent对象，其中包含有关文本事件的信息
         * @param type 事件的类型，可以作为TextEvent::type访问
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段。默认值为false
         * @param cancelable 确定是否可以取消Event对象。默认值为false
         * @param text 用户输入的一个或多个文本字符。事件侦听器可以通过text属性访问此信息
         * @version Egret 2.4
         * @platform Web,Native
         */
        explicit TextEvent(const std::string& type, bool bubbles = false, bool cancelable = false, const std::string& text = "");
        
        /**
         * 虚析构函数
         */
        virtual ~TextEvent() = default;
        
        // ========== 公开属性访问器 ==========
        
        /**
         * 在TextEvent::LINK事件中，event对应的字符串
         * @version Egret 2.4
         * @platform Web,Native
         */
        const std::string& getText() const { return m_text; }
        
        /**
         * 设置文本内容（内部使用）
         * @param text 文本内容
         */
        void setText(const std::string& text) { m_text = text; }
        
        // ========== 静态工厂方法 ==========
        
        /**
         * 从对象池中取出或创建一个新的TextEvent实例
         * @param type 事件的类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @param cancelable 确定是否可以取消Event对象
         * @param text 用户输入的文本字符
         * @return TextEvent实例
         */
        static std::shared_ptr<TextEvent> create(const std::string& type, bool bubbles = false, bool cancelable = false, const std::string& text = "");
        
        /**
         * 释放一个TextEvent对象，并缓存到对象池
         * @param event 要释放的事件对象（必须是由TextEvent::create()创建的）
         */
        static void release(std::shared_ptr<TextEvent> event);
        
        /**
         * 派发文本事件的便捷方法
         * @param target 派发事件目标
         * @param type 事件类型
         * @param text 文本内容
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @return 事件是否成功派发
         */
        static bool dispatchTextEvent(IEventDispatcher* target, const std::string& type, const std::string& text = "", bool bubbles = false);
        
    protected:
        /**
         * 清理方法，用于对象池复用时重置状态
         */
        virtual void clean() override;
        
    private:
        /**
         * 文本内容
         * 在TextEvent::LINK事件中，此属性包含用户单击的超链接的文本
         */
        std::string m_text;
        
        /** TextEvent对象池 */
        static std::vector<std::shared_ptr<TextEvent>> s_textEventPool;
    };
    
} // namespace egret