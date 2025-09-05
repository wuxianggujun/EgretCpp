#pragma once
#include <glm/vec2.hpp>
#include <memory>
#include <vector>

namespace egret {

    /**
     * Point类表示二维坐标系中的一个点
     * 对应TypeScript: export class Point
     */
    class Point {
    public:
        /**
         * 构造函数
         * @param x X坐标
         * @param y Y坐标
         */
        Point(double x = 0.0, double y = 0.0);
        
        /**
         * 拷贝构造函数
         */
        Point(const Point& other);
        
        /**
         * 从GLM vec2构造
         */
        Point(const glm::dvec2& vec);
        
        /**
         * 析构函数
         */
        ~Point() = default;

        // ========== 属性访问 ==========
        
        /**
         * X坐标
         */
        double getX() const { return m_vec.x; }
        void setX(double value) { m_vec.x = value; }
        
        /**
         * Y坐标
         */
        double getY() const { return m_vec.y; }
        void setY(double value) { m_vec.y = value; }
        
        /**
         * 长度（距离原点的距离）
         */
        double getLength() const;
        
        // ========== 操作方法 ==========
        
        /**
         * 设置点的坐标
         */
        void setTo(double x, double y);
        
        /**
         * 复制另一个点的坐标
         */
        void copyFrom(const Point& other);
        
        /**
         * 克隆当前点
         */
        Point clone() const;
        
        /**
         * 将点归一化（长度变为1）
         */
        void normalize();
        
        /**
         * 偏移点的位置
         */
        void offset(double dx, double dy);
        
        /**
         * 检查两点是否相等
         */
        bool equals(const Point& other) const;
        
        /**
         * 计算两点间的距离
         */
        static double distance(const Point& pt1, const Point& pt2);
        
        /**
         * 插值计算两点间的中间点
         */
        static Point interpolate(const Point& pt1, const Point& pt2, double f);
        
        /**
         * 计算极坐标点
         */
        static Point polar(double len, double angle);
        
        // ========== 运算符重载 ==========
        
        Point& operator=(const Point& other);
        Point operator+(const Point& other) const;
        Point operator-(const Point& other) const;
        Point operator*(double scalar) const;
        Point operator/(double scalar) const;
        
        Point& operator+=(const Point& other);
        Point& operator-=(const Point& other);
        Point& operator*=(double scalar);
        Point& operator/=(double scalar);
        
        bool operator==(const Point& other) const;
        bool operator!=(const Point& other) const;
        
        // ========== GLM集成 ==========
        
        /**
         * 获取GLM vec2表示
         */
        const glm::dvec2& getVec() const { return m_vec; }
        glm::dvec2& getVec() { return m_vec; }
        
        /**
         * 转换为GLM vec2
         */
        operator glm::dvec2() const { return m_vec; }
        
        // ========== 对象池支持 ==========
        
        /**
         * 创建Point对象（使用对象池）
         */
        static std::shared_ptr<Point> create(double x = 0.0, double y = 0.0);
        
        /**
         * 回收Point对象到对象池
         */
        static void release(std::shared_ptr<Point> point);

    private:
        glm::dvec2 m_vec;
        
        // 对象池相关
        static const size_t POOL_SIZE = 100;
        static thread_local std::vector<std::shared_ptr<Point>> s_pool;
        static thread_local size_t s_poolIndex;
    };

} // namespace egret