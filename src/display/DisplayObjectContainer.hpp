#pragma once
#include "DisplayObject.hpp"
#include <vector>
#include <memory>
#include <string>

namespace egret {

    // 前置声明
    class Stage;
    class Rectangle;

    /**
     * DisplayObjectContainer 类是基本显示列表构造块：一个可包含子项的显示列表节点。
     * 对应TypeScript: export class DisplayObjectContainer extends DisplayObject
     * 完整翻译自egret-core-5.4.1/src/egret/display/DisplayObjectContainer.ts
     */
    class DisplayObjectContainer : public DisplayObject {
    public:
        DisplayObjectContainer();
        virtual ~DisplayObjectContainer() = default;
        
        // ========== 基本属性访问 ==========
        
        /**
         * 返回此对象的子项数目
         */
        int getNumChildren() const { return static_cast<int>(m_children.size()); }
        
        // ========== 子对象管理 ==========
        
        /**
         * 将一个 DisplayObject 子实例添加到该 DisplayObjectContainer 实例中
         */
        virtual DisplayObject* addChild(DisplayObject* child);
        
        /**
         * 将一个 DisplayObject 子实例添加到该 DisplayObjectContainer 实例的指定索引位置
         */
        virtual DisplayObject* addChildAt(DisplayObject* child, int index);
        
        /**
         * 确定指定显示对象是 DisplayObjectContainer 实例的子项或该实例本身
         */
        bool contains(DisplayObject* child) const;
        
        /**
         * 返回位于指定索引处的子显示对象实例
         */
        DisplayObject* getChildAt(int index) const;
        
        /**
         * 返回 DisplayObject 的 child 实例的索引位置
         */
        int getChildIndex(DisplayObject* child) const;
        
        /**
         * 返回具有指定名称的子显示对象
         */
        DisplayObject* getChildByName(const std::string& name) const;
        
        /**
         * 从 DisplayObjectContainer 实例的子列表中删除指定的 child DisplayObject 实例
         */
        virtual DisplayObject* removeChild(DisplayObject* child);
        
        /**
         * 从 DisplayObjectContainer 的子列表中指定的 index 位置删除子 DisplayObject
         */
        DisplayObject* removeChildAt(int index);
        
        /**
         * 更改现有子项在显示对象容器中的位置
         */
        void setChildIndex(DisplayObject* child, int index);
        
        /**
         * 在子级列表中两个指定的索引位置，交换子对象的 Z 轴顺序
         */
        void swapChildrenAt(int index1, int index2);
        
        /**
         * 交换两个指定子对象的 Z 轴顺序
         */
        void swapChildren(DisplayObject* child1, DisplayObject* child2);
        
        /**
         * 从 DisplayObjectContainer 实例的子级列表中删除所有 child DisplayObject 实例
         */
        void removeChildren();
        
        // ========== 触摸控制 ==========
        
        /**
         * 确定对象的子级是否支持触摸或用户输入设备
         */
        bool getTouchChildren() const { return m_touchChildren; }
        void setTouchChildren(bool value) { m_touchChildren = value; }
        
        // ========== 静态事件列表管理 ==========
        
        /**
         * 待添加到舞台的对象列表（静态全局）
         */
        static std::vector<DisplayObject*> s_eventAddToStageList;
        
        /**
         * 待从舞台移除的对象列表（静态全局）
         */
        static std::vector<DisplayObject*> s_eventRemoveFromStageList;
        
        // ========== 重写的显示对象方法 ==========
        
        /**
         * 点碰撞检测 - 重写基类方法以支持容器层次结构
         */
        bool hitTestPoint(double x, double y, bool shapeFlag = false) override;
        
        /**
         * 命中测试 - 递归检测子对象
         */
        DisplayObject* hitTest(double stageX, double stageY);
        
        /**
         * 添加到舞台时的处理 - 重写基类方法
         */
        void onAddToStage(Stage* stage, int nestLevel) override;
        
        /**
         * 从舞台移除时的处理 - 重写基类方法  
         */
        void onRemoveFromStage() override;
        
    protected:
        /**
         * 测量子对象边界
         */
        void measureChildBounds(Rectangle& bounds) const;
        
        /**
         * 子项被添加时的回调（可被子类重写）
         */
        virtual void onChildAdded(DisplayObject* child, int index) {}
        
        /**
         * 子项被移除时的回调（可被子类重写）
         */
        virtual void onChildRemoved(DisplayObject* child, int index) {}
        
    private:
        /**
         * 子对象列表
         */
        std::vector<DisplayObject*> m_children;
        
        /**
         * 是否支持子对象触摸
         */
        bool m_touchChildren = true;
        
        // ========== 私有实现方法 ==========
        
        /**
         * 内部添加子对象方法
         */
        DisplayObject* doAddChild(DisplayObject* child, int index, bool notifyListeners = true);
        
        /**
         * 内部移除子对象方法
         */
        DisplayObject* doRemoveChild(int index, bool notifyListeners = true);
        
        /**
         * 内部设置子对象索引方法
         */
        void doSetChildIndex(DisplayObject* child, int index);
        
        /**
         * 内部交换子对象方法
         */
        void doSwapChildrenAt(int index1, int index2);
    };

} // namespace egret