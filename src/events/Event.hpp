//
// Created by wuxianggujun on 25-9-4.
//

#pragma once
#include <any>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "events/EventPhase.hpp"
#include "core/HashObject.hpp"

namespace egret
{
    // 前向声明
    class IEventDispatcher;
    
    /**
     * Event类是创建事件实例的基类，当发生事件时，Event实例将作为参数传递给事件侦听器。
     * Event类的属性包含有关事件的基本信息，例如事件的类型或者是否可以取消事件的默认行为。
     */
    class Event : public HashObject
    {

    public:
        // ========== 事件类型常量 ==========
        
        /** 在将显示对象直接添加到舞台显示列表或将包含显示对象的子树添加至舞台显示列表中时调度 */
        static const std::string ADDED_TO_STAGE;
        /** 在从显示列表中直接删除显示对象或删除包含显示对象的子树时调度 */
        static const std::string REMOVED_FROM_STAGE;
        /** 将显示对象添加到显示列表中时调度 */
        static const std::string ADDED;
        /** 将要从显示列表中删除显示对象时调度 */
        static const std::string REMOVED;
        /** 进入新的一帧,监听此事件将会在下一帧开始时触发一次回调 */
        static const std::string ENTER_FRAME;
        /** 渲染事件，监听此事件将会在本帧末即将开始渲染的前一刻触发回调 */
        static const std::string RENDER;
        /** 舞台尺寸或UI组件尺寸发生改变 */
        static const std::string RESIZE;
        /** 属性值或状态发生改变 */
        static const std::string CHANGE;
        /** 属性值或状态即将发生改变 */
        static const std::string CHANGING;
        /** 网络请求加载完成 */
        static const std::string COMPLETE;
        /** 循环完成 */
        static const std::string LOOP_COMPLETE;
        /** TextInput实例获得焦点 */
        static const std::string FOCUS_IN;
        /** TextInput实例失去焦点 */
        static const std::string FOCUS_OUT;
        /** 动画声音等播放完成 */
        static const std::string ENDED;
        /** 游戏激活 */
        static const std::string ACTIVATE;
        /** 取消激活 */
        static const std::string DEACTIVATE;
        /** 关闭事件 */
        static const std::string CLOSE;
        /** 连接事件 */
        static const std::string CONNECT;
        /** 离开舞台 */
        static const std::string LEAVE_STAGE;
        /** 声音播放完成 */
        static const std::string SOUND_COMPLETE;

        // ========== 构造函数 ==========
        
        /**
         * 创建一个作为参数传递给事件侦听器的Event对象
         * @param type 事件的类型，可以作为Event.type访问
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段，默认值为false
         * @param cancelable 确定是否可以取消Event对象，默认值为false
         * @param data 与此事件对象关联的可选数据
         */
        explicit Event(const std::string& type, bool bubbles = false, bool cancelable = false, const std::any& data = std::any());

        virtual ~Event() = default;
        
        // ========== 公开属性访问器 ==========
        
        /**
         * 与此事件对象关联的可选数据
         */
        std::any data;

        /**
         * 事件的类型。类型区分大小写
         */
        const std::string& getType() const { return m_type; }
        
        /**
         * 表示事件是否为冒泡事件。如果事件可以冒泡，则此值为true；否则为false
         */
        bool getBubbles() const { return m_bubbles; }
        
        /**
         * 表示是否可以阻止与事件相关联的行为。如果可以取消该行为，则此值为true；否则为false
         */
        bool getCancelable() const { return m_cancelable; }
        
        /**
         * 事件流中的当前阶段
         * 此属性可以包含以下数值：
         * 捕获阶段(EventPhase::CAPTURING_PHASE)
         * 目标阶段(EventPhase::AT_TARGET)
         * 冒泡阶段(EventPhase::BUBBLING_PHASE)
         */
        EventPhase getEventPhase() const { return m_eventPhase; }
        
        /**
         * 当前正在使用某个事件侦听器处理Event对象的对象
         */
        const std::any& getCurrentTarget() const { return m_currentTarget; }
        
        /**
         * 事件目标。此属性包含目标节点
         */
        const std::any& getTarget() const { return m_target; }
        
        // ========== 事件控制方法 ==========
        
        /**
         * 检查是否已对事件调用preventDefault()方法
         * @return 如果已调用preventDefault()方法，则返回true；否则返回false
         */
        bool isDefaultPrevented() const { return m_isDefaultPrevented; }
        
        /**
         * 如果可以取消事件的默认行为，则取消该行为
         * 许多事件都有默认执行的关联行为。您可以使用Event.cancelable属性来检查是否可以防止与特定事件关联的默认行为
         */
        void preventDefault();
        
        /**
         * 防止对事件流中当前节点的后续节点中的所有事件侦听器进行处理
         * 此方法不会影响当前节点currentTarget中的任何事件侦听器
         */
        void stopPropagation();
        
        /**
         * 防止对事件流中当前节点中和所有后续节点中的事件侦听器进行处理
         * 此方法会立即生效，并且会影响当前节点中的事件侦听器
         */
        void stopImmediatePropagation();
        
        // ========== 内部状态查询方法 ==========
        
        /**
         * 检查事件传播是否已停止
         */
        bool isPropagationStopped() const { return m_isPropagationStopped; }
        
        /**
         * 检查事件传播是否已立即停止
         */
        bool isPropagationImmediateStopped() const { return m_isPropagationImmediateStopped; }
        
        // ========== 内部方法（供事件系统使用）==========
        
        /**
         * 设置事件目标（内部使用）
         * @param target 事件目标
         * @return 总是返回true
         */
        virtual bool setTarget(const std::any& target);
        
        /**
         * 设置当前目标（内部使用）
         * @param currentTarget 当前处理事件的对象
         */
        void setCurrentTarget(const std::any& currentTarget) {
            m_currentTarget = currentTarget;
        }

        /**
         * 设置事件阶段（内部使用）
         * @param eventPhase 事件阶段
         */
        void setEventPhase(EventPhase eventPhase) {
            m_eventPhase = eventPhase;
        }
        
        // ========== 静态工厂方法和对象池 ==========
        
        /**
         * 从对象池中取出或创建一个新的事件实例
         * 我们建议您尽可能使用Event::create()和Event::release()这一对方法来创建和释放事件对象
         * @param type 事件的类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @param cancelable 确定是否可以取消Event对象
         * @return 事件实例
         */
        static std::shared_ptr<Event> create(const std::string& type, bool bubbles = false, bool cancelable = false);
        
        /**
         * 释放一个事件对象，并缓存到对象池
         * @param event 要释放的事件对象（必须是由Event::create()创建的）
         */
        static void release(std::shared_ptr<Event> event);
        
        /**
         * 使用指定的EventDispatcher对象来抛出Event事件对象
         * 抛出的对象将会缓存在对象池上，供下次循环复用
         * @param target 派发事件目标
         * @param type 事件类型
         * @param bubbles 确定Event对象是否参与事件流的冒泡阶段
         * @param data 事件data
         * @return 事件是否成功派发
         */
        static bool dispatchEvent(IEventDispatcher* target, const std::string& type, bool bubbles = false, const std::any& data = std::any());
        
        /**
         * 获取属性数据（内部使用）
         * @param eventClass 事件类
         * @return 属性数据映射
         */
        static std::unordered_map<std::string, std::any>& getPropertyData(const std::string& eventClass);

    protected:
        // ========== 受保护的清理方法 ==========
        
        /**
         * 当事件实例传递给Event::release()静态方法时，实例上的clean()方法将会被自动调用
         * 若此自定义事件的实例设计为可以循环复用的，为了避免引起内存泄露，自定义事件需要覆盖此方法来确保实例被缓存前断开对外部对象的一切引用
         */
        virtual void clean();
        
    private:
        // ========== 私有成员变量 ==========
        
        std::string m_type;                           // 事件类型
        bool m_bubbles;                              // 是否冒泡
        bool m_cancelable;                           // 是否可取消
        EventPhase m_eventPhase;                     // 事件阶段
        std::any m_currentTarget;                    // 当前目标
        std::any m_target;                          // 事件目标
        bool m_isDefaultPrevented;                   // 是否已阻止默认行为
        bool m_isPropagationStopped;                 // 是否已停止传播
        bool m_isPropagationImmediateStopped;        // 是否已立即停止传播
        
        // ========== 静态成员变量 ==========
        
        /** 事件对象池 */
        static std::vector<std::shared_ptr<Event>> s_eventPool;
        
        /** 属性数据映射（用于对象池复用时的属性存储） */
        static std::unordered_map<std::string, std::unordered_map<std::string, std::any>> s_propertyData;
    };
} // egret
