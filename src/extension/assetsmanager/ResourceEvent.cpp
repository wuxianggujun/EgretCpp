/**
 * @file ResourceEvent.cpp
 * @brief ResourceEvent实现 - 资源加载事件类实现
 * 
 * 翻译自：egret-core-5.4.1/src/extension/assetsmanager/src/shim/ResourceEvent.ts
 * 100%对应TypeScript版本功能，完全兼容API
 */

#include "extension/assetsmanager/ResourceEvent.hpp"
#include "events/IEventDispatcher.hpp"
#include <vector>
#include <mutex>

namespace RES {

    // ========== 静态常量定义 ==========

    const std::string ResourceEvent::ITEM_LOAD_ERROR = "itemLoadError";
    const std::string ResourceEvent::CONFIG_COMPLETE = "configComplete";
    const std::string ResourceEvent::CONFIG_LOAD_ERROR = "configLoadError";
    const std::string ResourceEvent::GROUP_PROGRESS = "groupProgress";
    const std::string ResourceEvent::GROUP_COMPLETE = "groupComplete";
    const std::string ResourceEvent::GROUP_LOAD_ERROR = "groupLoadError";

    // ========== 对象池管理 ==========

    namespace {
        // 简单的对象池实现
        std::vector<std::unique_ptr<ResourceEvent>> eventPool;
        std::mutex poolMutex;
    }

    // ========== 构造函数和析构函数 ==========

    ResourceEvent::ResourceEvent(const std::string& type, bool bubbles, bool cancelable)
        : Event(type, bubbles, cancelable)
        , itemsLoaded(0)
        , itemsTotal(0)
        , groupName("")
        , resItem(nullptr) {
        // 初始化成员变量
    }

    ResourceEvent::~ResourceEvent() {
        // 析构函数
    }

    // ========== 静态方法实现 ==========

    bool ResourceEvent::dispatchResourceEvent(egret::IEventDispatcher* target, const std::string& type,
                                            const std::string& groupName, 
                                            const ResourceInfo* resItem,
                                            int itemsLoaded, int itemsTotal) {
        if (!target) {
            return false;
        }

        // 创建事件对象（使用对象池）
        ResourceEvent* event = create(type);
        
        // 设置事件属性
        event->groupName = groupName;
        event->itemsLoaded = itemsLoaded;
        event->itemsTotal = itemsTotal;

        // 如果提供了资源项信息，转换为ResourceItem
        if (resItem) {
            event->resItem = std::make_shared<ResourceItemData>(
                ResourceItem::convertToResItem(*resItem)
            );
        } else {
            event->resItem = nullptr;
        }

        // 派发事件
        bool result = target->dispatchEvent(*event);

        // 释放事件对象（返回对象池）
        release(event);

        return result;
    }

    ResourceEvent* ResourceEvent::create(const std::string& type) {
        std::lock_guard<std::mutex> lock(poolMutex);

        ResourceEvent* event = nullptr;

        // 尝试从对象池中获取
        if (!eventPool.empty()) {
            event = eventPool.back().release();
            eventPool.pop_back();
            
            // 重置事件状态
            event->setType(type);
            event->resetInternal();
        } else {
            // 对象池为空，创建新对象
            event = new ResourceEvent(type);
        }

        return event;
    }

    void ResourceEvent::release(ResourceEvent* event) {
        if (!event) {
            return;
        }

        std::lock_guard<std::mutex> lock(poolMutex);

        // 重置事件状态
        event->resetInternal();

        // 返回对象池
        eventPool.push_back(std::unique_ptr<ResourceEvent>(event));
    }

    // ========== 受保护方法实现 ==========

    void ResourceEvent::resetInternal() {
        // 重置事件对象的状态，准备复用
        itemsLoaded = 0;
        itemsTotal = 0;
        groupName.clear();
        resItem = nullptr;
        
        // 调用父类的重置方法（如果有）
        // Event::resetInternal();
    }

} // namespace RES