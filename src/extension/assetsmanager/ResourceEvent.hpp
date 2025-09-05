/**
 * @file ResourceEvent.hpp
 * @brief ResourceEvent类 - 资源加载事件，继承自Event类
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/shim/ResourceEvent.ts
 * ResourceEvent是资源管理系统的事件类，用于通知资源加载的各种状态和进度。
 * 支持单个资源加载、资源组加载、配置文件加载等多种场景的事件通知。
 */

#pragma once

#include "events/Event.hpp"
#include "extension/assetsmanager/ResourceItem.hpp"
#include <string>
#include <memory>

namespace egret {
class IEventDispatcher;
}

namespace RES {

/**
 * @brief ResourceEvent类 - 资源加载事件
 * 
 * 资源加载事件类，继承自egret::Event，用于在资源加载过程中
 * 通知加载进度、完成状态、错误信息等。
 * 
 * @version Egret 5.2
 * @note 对应TypeScript的RES.ResourceEvent类，100%API兼容
 */
class ResourceEvent : public egret::Event {
public:
    // ========== 事件类型常量 ==========

    /**
     * @brief 一个加载项加载失败事件
     * 
     * 当单个资源项加载失败时派发此事件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    static const std::string ITEM_LOAD_ERROR;

    /**
     * @brief 配置文件加载并解析完成事件
     * 
     * 注意：若有配置文件加载失败，将不会抛出此事件，
     * 若要处理配置加载失败，请同时监听CONFIG_LOAD_ERROR事件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    static const std::string CONFIG_COMPLETE;

    /**
     * @brief 配置文件加载失败事件
     * 
     * 当resource.json等配置文件加载失败时派发此事件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    static const std::string CONFIG_LOAD_ERROR;

    /**
     * @brief 延迟加载组资源加载进度事件
     * 
     * 在资源组加载过程中定期派发，报告加载进度。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    static const std::string GROUP_PROGRESS;

    /**
     * @brief 延迟加载组资源加载完成事件
     * 
     * 注意：若组内有资源项加载失败，将不会抛出此事件，
     * 若要处理组加载失败，请同时监听GROUP_LOAD_ERROR事件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    static const std::string GROUP_COMPLETE;

    /**
     * @brief 延迟加载组资源加载失败事件
     * 
     * 当资源组中有资源加载失败时派发此事件。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    static const std::string GROUP_LOAD_ERROR;

    // ========== 构造函数和析构函数 ==========

    /**
     * @brief 构造函数 - 创建ResourceEvent事件对象
     * 
     * 创建一个作为参数传递给事件侦听器的ResourceEvent对象。
     * 
     * @param type 事件的类型，可以作为Event.type访问
     * @param bubbles 确定Event对象是否参与事件流的冒泡阶段，默认值为false
     * @param cancelable 确定是否可以取消Event对象，默认值为false
     * @version Egret 5.2
     */
    ResourceEvent(const std::string& type, bool bubbles = false, bool cancelable = false);

    /**
     * @brief 析构函数
     */
    virtual ~ResourceEvent();

    // ========== 公共属性 ==========

    /**
     * @brief 已经加载的文件数
     * 
     * 在进度事件中表示已完成加载的资源数量。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    int itemsLoaded;

    /**
     * @brief 要加载的总文件数
     * 
     * 在进度事件中表示需要加载的资源总数量。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    int itemsTotal;

    /**
     * @brief 资源组名
     * 
     * 在组相关事件中表示操作的资源组名称。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    std::string groupName;

    /**
     * @brief 一次加载项加载结束的项信息对象
     * 
     * 在单个资源加载完成或失败时，包含该资源的详细信息。
     * 
     * @version Egret 5.2
     * @platform Web,Native
     */
    std::shared_ptr<ResourceItemData> resItem;

    // ========== 静态方法 ==========

    /**
     * @brief 派发资源事件
     * 
     * 使用指定的EventDispatcher对象来抛出事件对象。
     * 抛出的对象将会缓存在对象池上，供下次循环复用。
     * 
     * @param target 事件派发目标
     * @param type 事件类型
     * @param groupName 资源组名，默认为空字符串
     * @param resItem 资源项信息，默认为nullptr
     * @param itemsLoaded 已加载项数，默认为0
     * @param itemsTotal 总项数，默认为0
     * @return bool 事件是否成功派发
     * @version Egret 5.2
     */
    static bool dispatchResourceEvent(egret::IEventDispatcher* target, const std::string& type,
                                    const std::string& groupName = "", 
                                    const ResourceInfo* resItem = nullptr,
                                    int itemsLoaded = 0, int itemsTotal = 0);

    /**
     * @brief 创建ResourceEvent对象（对象池模式）
     * 
     * 从对象池中获取或创建新的ResourceEvent对象。
     * 
     * @param type 事件类型
     * @return ResourceEvent* 事件对象指针
     * @version Egret 5.2
     */
    static ResourceEvent* create(const std::string& type);

    /**
     * @brief 释放ResourceEvent对象（对象池模式）
     * 
     * 将ResourceEvent对象返回到对象池中。
     * 
     * @param event 要释放的事件对象
     * @version Egret 5.2
     */
    static void release(ResourceEvent* event);

protected:
    /**
     * @brief 重置事件对象状态
     * 
     * 在对象池复用时重置事件对象的状态。
     */
    void resetInternal();

private:
    // 禁用拷贝构造和赋值操作
    ResourceEvent(const ResourceEvent&) = delete;
    ResourceEvent& operator=(const ResourceEvent&) = delete;
};

} // namespace RES