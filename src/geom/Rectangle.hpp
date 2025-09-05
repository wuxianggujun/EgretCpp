#pragma once
#include "geom/Point.hpp"
#include <memory>
#include <vector>

namespace egret {

    /**
     * Rectangle类表示一个矩形区域
     * 对应TypeScript: export class Rectangle
     */
    class Rectangle {
    public:
        /**
         * 构造函数
         * @param x X坐标
         * @param y Y坐标
         * @param width 宽度
         * @param height 高度
         */
        Rectangle(double x = 0.0, double y = 0.0, double width = 0.0, double height = 0.0);
        
        /**
         * 拷贝构造函数
         */
        Rectangle(const Rectangle& other);
        
        /**
         * 析构函数
         */
        ~Rectangle() = default;

        // ========== 属性访问 ==========
        
        /**
         * X坐标
         */
        double getX() const { return m_x; }
        void setX(double value) { m_x = value; }
        
        /**
         * Y坐标
         */
        double getY() const { return m_y; }
        void setY(double value) { m_y = value; }
        
        /**
         * 宽度
         */
        double getWidth() const { return m_width; }
        void setWidth(double value) { m_width = value; }
        
        /**
         * 高度
         */
        double getHeight() const { return m_height; }
        void setHeight(double value) { m_height = value; }
        
        // ========== 直接属性访问（兼容性） ==========
        
        /**
         * 直接访问X坐标 - 为了兼容性提供公共属性访问
         */
        double x = 0.0;
        
        /**
         * 直接访问Y坐标 - 为了兼容性提供公共属性访问
         */
        double y = 0.0;
        
        /**
         * 直接访问宽度 - 为了兼容性提供公共属性访问
         */
        double width = 0.0;
        
        /**
         * 直接访问高度 - 为了兼容性提供公共属性访问
         */
        double height = 0.0;
        
        /**
         * 左边界
         */
        double getLeft() const { return m_x; }
        void setLeft(double value);
        
        /**
         * 右边界
         */
        double getRight() const { return m_x + m_width; }
        void setRight(double value);
        
        /**
         * 顶部边界
         */
        double getTop() const { return m_y; }
        void setTop(double value);
        
        /**
         * 底部边界
         */
        double getBottom() const { return m_y + m_height; }
        void setBottom(double value);
        
        /**
         * 左上角点
         */
        Point getTopLeft() const { return Point(m_x, m_y); }
        void setTopLeft(const Point& point);
        
        /**
         * 右下角点
         */
        Point getBottomRight() const { return Point(m_x + m_width, m_y + m_height); }
        void setBottomRight(const Point& point);
        
        /**
         * 大小（作为Point）
         */
        Point getSize() const { return Point(m_width, m_height); }
        void setSize(const Point& size);
        
        // ========== 操作方法 ==========
        
        /**
         * 设置矩形的所有属性
         */
        void setTo(double x, double y, double width, double height);
        
        /**
         * 设置矩形的所有属性（别名方法）
         */
        void setRect(double x, double y, double width, double height) {
            setTo(x, y, width, height);
        }
        
        /**
         * 复制另一个矩形的属性
         */
        void copyFrom(const Rectangle& other);
        
        /**
         * 克隆当前矩形
         */
        Rectangle clone() const;
        
        /**
         * 检查两矩形是否相等
         */
        bool equals(const Rectangle& other) const;
        
        /**
         * 检查矩形是否为空
         */
        bool isEmpty() const;
        
        /**
         * 设置矩形为空
         */
        void setEmpty();
        
        /**
         * 膨胀矩形
         */
        void inflate(double dx, double dy);
        
        /**
         * 按指定点膨胀
         */
        void inflatePoint(const Point& point);
        
        /**
         * 偏移矩形位置
         */
        void offset(double dx, double dy);
        
        /**
         * 按指定点偏移
         */
        void offsetPoint(const Point& point);
        
        // ========== 碰撞检测 ==========
        
        /**
         * 检查点是否在矩形内
         */
        bool contains(double x, double y) const;
        
        /**
         * 检查点是否在矩形内
         */
        bool containsPoint(const Point& point) const;
        
        /**
         * 检查矩形是否完全包含另一个矩形
         */
        bool containsRect(const Rectangle& rect) const;
        
        /**
         * 检查两矩形是否相交
         */
        bool intersects(const Rectangle& rect) const;
        
        /**
         * 获取两矩形的交集
         */
        Rectangle intersection(const Rectangle& rect) const;
        
        /**
         * 计算当前矩形与另一个矩形的交集（修改当前矩形）
         */
        Rectangle& intersectsWith(const Rectangle& rect);
        
        /**
         * 获取两矩形的并集
         */
        Rectangle unionRect(const Rectangle& rect) const;
        
        /**
         * 计算当前矩形与另一个矩形的并集（修改当前矩形）
         */
        Rectangle& unionWith(const Rectangle& rect);
        
        /**
         * 扩展矩形以包含指定点
         */
        Rectangle& unionWithPoint(const Point& point);
        
        // ========== 运算符重载 ==========
        
        Rectangle& operator=(const Rectangle& other);
        bool operator==(const Rectangle& other) const;
        bool operator!=(const Rectangle& other) const;
        
        // ========== 对象池支持 ==========
        
        /**
         * 创建Rectangle对象（使用对象池）
         */
        static std::shared_ptr<Rectangle> create(double x = 0.0, double y = 0.0, double width = 0.0, double height = 0.0);
        
        /**
         * 回收Rectangle对象到对象池
         */
        static void release(std::shared_ptr<Rectangle> rect);

    private:
        double m_x;
        double m_y;
        double m_width;
        double m_height;
        
        // 对象池相关
        static const size_t POOL_SIZE = 50;
        static thread_local std::vector<std::shared_ptr<Rectangle>> s_pool;
        static thread_local size_t s_poolIndex;
    };

} // namespace egret