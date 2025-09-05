#pragma once
#include "display/DisplayObject.hpp"
#include "display/Graphics.hpp"
#include "geom/Rectangle.hpp"
#include <memory>

namespace egret {

    /**
     * Shape类用于使用绘图应用程序编程接口(API)创建简单形状
     * Shape类含有graphics属性，通过该属性您可以访问各种矢量绘图方法
     * 基于TypeScript egret.display.Shape完整实现
     * 
     * @see Graphics
     * @version Egret 2.4
     * @platform Web,Native
     */
    class Shape : public DisplayObject {
    public:
        /**
         * 创建一个Shape对象
         * @version Egret 2.4
         * @platform Web,Native
         */
        Shape();
        
        /**
         * 析构函数
         */
        virtual ~Shape();

        // ========== Graphics访问方法 ==========
        
        /**
         * 获取Shape中的Graphics对象。可通过此对象执行矢量绘图命令。
         * @return Graphics对象指针
         * @version Egret 2.4
         * @platform Web,Native
         */
        Graphics* getGraphics() const { return m_graphics.get(); }

        // ========== DisplayObject重写方法 ==========
        
        /**
         * 测量内容边界
         * @param bounds 边界矩形，用于接收测量结果
         */
        void measureContentBounds(Rectangle& bounds) override;
        
        /**
         * 碰撞检测
         * @param stageX 舞台X坐标
         * @param stageY 舞台Y坐标
         * @return 碰撞的显示对象，如果没有碰撞返回nullptr
         */
        DisplayObject* hitTest(double stageX, double stageY) override;

    protected:
        // ========== 生命周期方法 ==========
        
        /**
         * 从舞台移除时调用
         */
        void onRemoveFromStage() override;

    private:
        // ========== 私有成员变量 ==========
        
        /**
         * Graphics对象，用于矢量绘图
         */
        std::unique_ptr<Graphics> m_graphics;

        // ========== 私有辅助方法 ==========
        
        /**
         * 初始化Graphics对象
         */
        void initializeGraphics();
    };

} // namespace egret