#include "display/Shape.hpp"
#include "display/DisplayObject.hpp"
#include "display/Graphics.hpp"
#include "geom/Rectangle.hpp"

namespace egret {

    // ========== 构造和析构 ==========
    
    Shape::Shape() : DisplayObject() {
        // 创建Graphics对象
        m_graphics = std::make_unique<Graphics>();
        
        // 初始化Graphics对象
        initializeGraphics();
    }

    Shape::~Shape() {
        // Graphics对象由智能指针自动管理
    }

    // ========== DisplayObject重写方法 ==========
    
    void Shape::measureContentBounds(Rectangle& bounds) {
        if (m_graphics) {
            m_graphics->measureContentBounds(bounds);
        } else {
            // 如果没有Graphics对象，设置空边界
            bounds.setTo(0, 0, 0, 0);
        }
    }
    
    DisplayObject* Shape::hitTest(double stageX, double stageY) {
        // 首先调用父类的碰撞检测
        DisplayObject* target = DisplayObject::hitTest(stageX, stageY);
        
        if (target == this) {
            // 如果基础碰撞检测通过，进一步检测Graphics内容
            if (m_graphics) {
                target = m_graphics->hitTest(stageX, stageY);
                // 如果Graphics碰撞检测失败，返回nullptr
                if (!target) {
                    target = nullptr;
                }
            }
        }
        
        return target;
    }

    // ========== 生命周期方法 ==========
    
    void Shape::onRemoveFromStage() {
        // 调用父类的移除方法
        DisplayObject::onRemoveFromStage();
        
        // 通知Graphics对象从舞台移除
        if (m_graphics) {
            m_graphics->onRemoveFromStage();
        }
    }

    // ========== 私有辅助方法 ==========
    
    void Shape::initializeGraphics() {
        if (m_graphics) {
            // 设置Graphics的目标显示对象为当前Shape
            m_graphics->setTarget(this);
        }
    }

} // namespace egret