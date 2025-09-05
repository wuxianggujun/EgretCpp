#include "DisplayObjectContainer.hpp"
#include "Stage.hpp"
#include "events/Event.hpp"
#include "geom/Rectangle.hpp"
#include "geom/Matrix.hpp"
#include <algorithm>
#include <stdexcept>

namespace egret {

    // 静态成员定义
    std::vector<DisplayObject*> DisplayObjectContainer::s_eventAddToStageList;
    std::vector<DisplayObject*> DisplayObjectContainer::s_eventRemoveFromStageList;

    DisplayObjectContainer::DisplayObjectContainer() : DisplayObject() {
        // 初始化子对象列表为空
    }

    // ========== 子对象管理实现 ==========

    DisplayObject* DisplayObjectContainer::addChild(DisplayObject* child) {
        int index = static_cast<int>(m_children.size());
        
        if (child->getParent() == this) {
            index--;
        }
        return doAddChild(child, index);
    }

    DisplayObject* DisplayObjectContainer::addChildAt(DisplayObject* child, int index) {
        if (index < 0 || index >= static_cast<int>(m_children.size())) {
            index = static_cast<int>(m_children.size());
            if (child->getParent() == this) {
                index--;
            }
        }
        return doAddChild(child, index);
    }

    bool DisplayObjectContainer::contains(DisplayObject* child) const {
        while (child) {
            if (child == this) {
                return true;
            }
            child = child->getParent();
        }
        return false;
    }

    DisplayObject* DisplayObjectContainer::getChildAt(int index) const {
        if (index >= 0 && index < static_cast<int>(m_children.size())) {
            return m_children[index];
        }
        return nullptr;
    }

    int DisplayObjectContainer::getChildIndex(DisplayObject* child) const {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end()) {
            return static_cast<int>(std::distance(m_children.begin(), it));
        }
        return -1;
    }

    DisplayObject* DisplayObjectContainer::getChildByName(const std::string& name) const {
        for (DisplayObject* child : m_children) {
            if (child->getName() == name) {
                return child;
            }
        }
        return nullptr;
    }

    DisplayObject* DisplayObjectContainer::removeChild(DisplayObject* child) {
        int index = getChildIndex(child);
        if (index >= 0) {
            return doRemoveChild(index);
        }
        return nullptr;
    }

    DisplayObject* DisplayObjectContainer::removeChildAt(int index) {
        if (index >= 0 && index < static_cast<int>(m_children.size())) {
            return doRemoveChild(index);
        }
        return nullptr;
    }

    void DisplayObjectContainer::setChildIndex(DisplayObject* child, int index) {
        if (index < 0 || index >= static_cast<int>(m_children.size())) {
            index = static_cast<int>(m_children.size()) - 1;
        }
        doSetChildIndex(child, index);
    }

    void DisplayObjectContainer::swapChildrenAt(int index1, int index2) {
        if (index1 >= 0 && index1 < static_cast<int>(m_children.size()) && 
            index2 >= 0 && index2 < static_cast<int>(m_children.size())) {
            doSwapChildrenAt(index1, index2);
        }
    }

    void DisplayObjectContainer::swapChildren(DisplayObject* child1, DisplayObject* child2) {
        int index1 = getChildIndex(child1);
        int index2 = getChildIndex(child2);
        if (index1 != -1 && index2 != -1) {
            doSwapChildrenAt(index1, index2);
        }
    }

    void DisplayObjectContainer::removeChildren() {
        while (!m_children.empty()) {
            doRemoveChild(static_cast<int>(m_children.size()) - 1);
        }
    }

    // ========== 命中测试实现 ==========

    bool DisplayObjectContainer::hitTestPoint(double x, double y, bool shapeFlag) {
        if (!getVisible()) {
            return false;
        }
        
        // 获取反向变换矩阵并转换坐标
        auto matrix = getInvertedConcatenatedMatrix();
        double localX = matrix->getA() * x + matrix->getC() * y + matrix->getTx();
        double localY = matrix->getB() * x + matrix->getD() * y + matrix->getTy();
        
        // 检查滚动矩形
        Rectangle* rect = getScrollRect();
        if (rect && !rect->contains(localX, localY)) {
            return false;
        }
        
        // 检查遮罩
        DisplayObject* mask = getMask();
        if (mask && !mask->hitTestPoint(x, y)) {
            return false;
        }
        
        // 检查子对象
        for (int i = static_cast<int>(m_children.size()) - 1; i >= 0; i--) {
            DisplayObject* child = m_children[i];
            if (child->hitTestPoint(x, y, shapeFlag)) {
                return true;
            }
        }
        
        // 调用父类的命中测试
        return DisplayObject::hitTestPoint(x, y, shapeFlag);
    }

    DisplayObject* DisplayObjectContainer::hitTest(double stageX, double stageY) {
        if (!getVisible()) {
            return nullptr;
        }
        
        // 获取反向变换矩阵并转换坐标
        auto matrix = getInvertedConcatenatedMatrix();
        double localX = matrix->getA() * stageX + matrix->getC() * stageY + matrix->getTx();
        double localY = matrix->getB() * stageX + matrix->getD() * stageY + matrix->getTy();
        
        // 检查滚动矩形
        Rectangle* rect = getScrollRect();
        if (rect && !rect->contains(localX, localY)) {
            return nullptr;
        }
        
        // 检查遮罩
        DisplayObject* mask = getMask();
        if (mask && !mask->hitTestPoint(stageX, stageY)) {
            return nullptr;
        }
        
        // 从最上层子对象开始测试
        bool found = false;
        DisplayObject* target = nullptr;
        for (int i = static_cast<int>(m_children.size()) - 1; i >= 0; i--) {
            DisplayObject* child = m_children[i];
            if (auto childContainer = dynamic_cast<DisplayObjectContainer*>(child)) {
                target = childContainer->hitTest(stageX, stageY);
            } else {
                if (child->hitTestPoint(stageX, stageY)) {
                    target = child;
                }
            }
            if (target) {
                found = true;
                if (target->getTouchEnabled()) {
                    break;
                } else {
                    target = nullptr;
                }
            }
        }
        
        if (target) {
            if (m_touchChildren) {
                return target;
            }
            return this;
        }
        
        if (found) {
            return this;
        }
        
        // 调用父类的命中测试
        return DisplayObject::hitTestPoint(stageX, stageY) ? this : nullptr;
    }

    // ========== 舞台生命周期管理 ==========

    void DisplayObjectContainer::onAddToStage(Stage* stage, int nestLevel) {
        DisplayObject::onAddToStage(stage, nestLevel);
        
        // 递归处理所有子对象
        nestLevel++;
        for (DisplayObject* child : m_children) {
            child->onAddToStage(stage, nestLevel);
        }
    }

    void DisplayObjectContainer::onRemoveFromStage() {
        DisplayObject::onRemoveFromStage();
        
        // 递归处理所有子对象
        for (DisplayObject* child : m_children) {
            child->onRemoveFromStage();
        }
    }

    // ========== 边界测量 ==========

    void DisplayObjectContainer::measureChildBounds(Rectangle& bounds) const {
        if (m_children.empty()) {
            return;
        }
        
        double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
        bool found = false;
        
        // 创建临时矩形
        Rectangle tempBounds;
        
        for (int i = -1; i < static_cast<int>(m_children.size()); i++) {
            Rectangle* childBounds;
            if (i == -1) {
                childBounds = &bounds;
            } else {
                tempBounds = m_children[i]->getBounds(nullptr, true);
                // 应用变换矩阵
                auto childMatrix = m_children[i]->getMatrix();
                // TODO: 实现矩阵变换边界的功能
                // childMatrix->transformBounds(tempBounds);
                childBounds = &tempBounds;
            }
            
            if (childBounds->isEmpty()) {
                continue;
            }
            
            if (found) {
                xMin = std::min(xMin, childBounds->getX());
                xMax = std::max(xMax, childBounds->getX() + childBounds->getWidth());
                yMin = std::min(yMin, childBounds->getY());
                yMax = std::max(yMax, childBounds->getY() + childBounds->getHeight());
            } else {
                found = true;
                xMin = childBounds->getX();
                xMax = xMin + childBounds->getWidth();
                yMin = childBounds->getY();
                yMax = yMin + childBounds->getHeight();
            }
        }
        
        bounds.setTo(xMin, yMin, xMax - xMin, yMax - yMin);
    }

    // ========== 私有实现方法 ==========

    DisplayObject* DisplayObjectContainer::doAddChild(DisplayObject* child, int index, bool notifyListeners) {
        if (!child) {
            return nullptr;
        }
        
        // 检查是否添加自身
        if (child == this) {
            // 错误：不能添加自身为子对象
            return nullptr;
        }
        
        // 检查是否形成循环引用
        if (auto container = dynamic_cast<DisplayObjectContainer*>(child)) {
            if (container->contains(this)) {
                // 错误：会形成循环引用
                return nullptr;
            }
        }
        
        DisplayObjectContainer* host = child->getParent();
        if (host == this) {
            doSetChildIndex(child, index);
            return child;
        }
        
        if (host) {
            host->removeChild(child);
        }
        
        // 插入到指定位置
        m_children.insert(m_children.begin() + index, child);
        child->setParentInternal(this);
        
        // 如果当前容器在舞台上，将子对象添加到舞台
        Stage* stage = getStage();
        if (stage) {
            child->onAddToStage(stage, getNestLevel() + 1);
        }
        
        // 分发ADDED事件
        if (notifyListeners) {
            auto addedEvent = Event::create(Event::ADDED, true);
            child->dispatchEvent(*addedEvent);
            Event::release(addedEvent);
        }
        
        // 处理ADDED_TO_STAGE事件列表
        if (stage) {
            while (!s_eventAddToStageList.empty()) {
                DisplayObject* childAddToStage = s_eventAddToStageList.front();
                s_eventAddToStageList.erase(s_eventAddToStageList.begin());
                
                if (childAddToStage->getStage() && notifyListeners) {
                    auto addedToStageEvent = Event::create(Event::ADDED_TO_STAGE);
                    childAddToStage->dispatchEvent(*addedToStageEvent);
                    Event::release(addedToStageEvent);
                }
            }
        }
        
        // 标记缓存为脏
        setCacheDirty(true);
        
        // 调用子类回调
        onChildAdded(child, index);
        
        return child;
    }

    DisplayObject* DisplayObjectContainer::doRemoveChild(int index, bool notifyListeners) {
        if (index < 0 || index >= static_cast<int>(m_children.size())) {
            return nullptr;
        }
        
        DisplayObject* child = m_children[index];
        
        // 调用子类回调
        onChildRemoved(child, index);
        
        // 分发REMOVED事件
        if (notifyListeners) {
            auto removedEvent = Event::create(Event::REMOVED, true);
            child->dispatchEvent(*removedEvent);
            Event::release(removedEvent);
        }
        
        // 如果在舞台上，处理移除
        if (getStage()) {
            child->onRemoveFromStage();
            
            // 处理REMOVED_FROM_STAGE事件列表
            while (!s_eventRemoveFromStageList.empty()) {
                DisplayObject* childRemoveFromStage = s_eventRemoveFromStageList.front();
                s_eventRemoveFromStageList.erase(s_eventRemoveFromStageList.begin());
                
                if (notifyListeners && childRemoveFromStage->getHasAddToStage()) {
                    auto removedFromStageEvent = Event::create(Event::REMOVED_FROM_STAGE);
                    childRemoveFromStage->dispatchEvent(*removedFromStageEvent);
                    Event::release(removedFromStageEvent);
                }
            }
        }
        
        // 重置父级关系
        child->setParentInternal(nullptr);
        
        // 从子对象列表中移除
        m_children.erase(m_children.begin() + index);
        
        // 标记缓存为脏
        setCacheDirty(true);
        
        return child;
    }

    void DisplayObjectContainer::doSetChildIndex(DisplayObject* child, int index) {
        int lastIndex = getChildIndex(child);
        if (lastIndex < 0) {
            // 子对象不存在
            return;
        }
        
        if (lastIndex == index) {
            return;
        }
        
        // 调用子类回调
        onChildRemoved(child, lastIndex);
        
        // 从原位置移除
        m_children.erase(m_children.begin() + lastIndex);
        
        // 插入到新位置
        m_children.insert(m_children.begin() + index, child);
        
        // 调用子类回调
        onChildAdded(child, index);
        
        // 标记缓存为脏
        setCacheDirty(true);
    }

    void DisplayObjectContainer::doSwapChildrenAt(int index1, int index2) {
        if (index1 > index2) {
            std::swap(index1, index2);
        } else if (index1 == index2) {
            return;
        }
        
        DisplayObject* child1 = m_children[index1];
        DisplayObject* child2 = m_children[index2];
        
        // 调用子类回调
        onChildRemoved(child1, index1);
        onChildRemoved(child2, index2);
        
        // 交换位置
        m_children[index1] = child2;
        m_children[index2] = child1;
        
        // 调用子类回调
        onChildAdded(child2, index1);
        onChildAdded(child1, index2);
        
        // 标记缓存为脏
        setCacheDirty(true);
    }

} // namespace egret