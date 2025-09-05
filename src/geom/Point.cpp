#include "geom/Point.hpp"
#include <glm/geometric.hpp>
#include <cmath>
#include <vector>

namespace egret {

    // 对象池静态成员定义
    thread_local std::vector<std::shared_ptr<Point>> Point::s_pool;
    thread_local size_t Point::s_poolIndex = 0;

    Point::Point(double x, double y) : m_vec(x, y) {
    }

    Point::Point(const Point& other) : m_vec(other.m_vec) {
    }

    Point::Point(const glm::dvec2& vec) : m_vec(vec) {
    }

    double Point::getLength() const {
        return glm::length(m_vec);
    }

    void Point::setTo(double x, double y) {
        m_vec.x = x;
        m_vec.y = y;
    }

    void Point::copyFrom(const Point& other) {
        m_vec = other.m_vec;
    }

    Point Point::clone() const {
        return Point(m_vec.x, m_vec.y);
    }

    void Point::normalize() {
        m_vec = glm::normalize(m_vec);
    }

    void Point::offset(double dx, double dy) {
        m_vec.x += dx;
        m_vec.y += dy;
    }

    bool Point::equals(const Point& other) const {
        const double epsilon = 1e-10;
        return std::abs(m_vec.x - other.m_vec.x) < epsilon && 
               std::abs(m_vec.y - other.m_vec.y) < epsilon;
    }

    double Point::distance(const Point& pt1, const Point& pt2) {
        return glm::distance(pt1.m_vec, pt2.m_vec);
    }

    Point Point::interpolate(const Point& pt1, const Point& pt2, double f) {
        return Point(glm::mix(pt1.m_vec, pt2.m_vec, f));
    }

    Point Point::polar(double len, double angle) {
        return Point(len * std::cos(angle), len * std::sin(angle));
    }

    // 运算符重载实现
    Point& Point::operator=(const Point& other) {
        if (this != &other) {
            m_vec = other.m_vec;
        }
        return *this;
    }

    Point Point::operator+(const Point& other) const {
        return Point(m_vec + other.m_vec);
    }

    Point Point::operator-(const Point& other) const {
        return Point(m_vec - other.m_vec);
    }

    Point Point::operator*(double scalar) const {
        return Point(m_vec * scalar);
    }

    Point Point::operator/(double scalar) const {
        return Point(m_vec / scalar);
    }

    Point& Point::operator+=(const Point& other) {
        m_vec += other.m_vec;
        return *this;
    }

    Point& Point::operator-=(const Point& other) {
        m_vec -= other.m_vec;
        return *this;
    }

    Point& Point::operator*=(double scalar) {
        m_vec *= scalar;
        return *this;
    }

    Point& Point::operator/=(double scalar) {
        m_vec /= scalar;
        return *this;
    }

    bool Point::operator==(const Point& other) const {
        return equals(other);
    }

    bool Point::operator!=(const Point& other) const {
        return !equals(other);
    }

    // 对象池实现
    std::shared_ptr<Point> Point::create(double x, double y) {
        if (s_pool.empty()) {
            s_pool.reserve(POOL_SIZE);
        }
        
        if (s_poolIndex < s_pool.size()) {
            auto point = s_pool[s_poolIndex++];
            point->setTo(x, y);
            return point;
        }
        
        auto point = std::make_shared<Point>(x, y);
        if (s_pool.size() < POOL_SIZE) {
            s_pool.push_back(point);
            s_poolIndex++;
        }
        return point;
    }

    void Point::release(std::shared_ptr<Point> point) {
        if (!point || s_poolIndex == 0) {
            return;
        }
        
        // 将点重置为原点并放回池中
        point->setTo(0.0, 0.0);
        s_poolIndex--;
        
        // 将该点移到池的开头位置
        if (s_poolIndex < s_pool.size()) {
            std::swap(s_pool[s_poolIndex], s_pool.back());
        }
    }

} // namespace egret