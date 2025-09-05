#include "geom/Rectangle.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace egret {

    // 对象池静态成员定义
    thread_local std::vector<std::shared_ptr<Rectangle>> Rectangle::s_pool;
    thread_local size_t Rectangle::s_poolIndex = 0;

    Rectangle::Rectangle(double x, double y, double width, double height)
        : m_x(x), m_y(y), m_width(width), m_height(height), x(x), y(y), width(width), height(height) {
    }

    Rectangle::Rectangle(const Rectangle& other)
        : m_x(other.m_x), m_y(other.m_y), m_width(other.m_width), m_height(other.m_height), 
          x(other.x), y(other.y), width(other.width), height(other.height) {
    }

    void Rectangle::setLeft(double value) {
        m_width += (m_x - value);
        m_x = value;
        x = m_x;
        width = m_width;
    }

    void Rectangle::setRight(double value) {
        m_width = value - m_x;
        width = m_width;
    }

    void Rectangle::setTop(double value) {
        m_height += (m_y - value);
        m_y = value;
        y = m_y;
        height = m_height;
    }

    void Rectangle::setBottom(double value) {
        m_height = value - m_y;
        height = m_height;
    }

    void Rectangle::setTopLeft(const Point& point) {
        setLeft(point.getX());
        setTop(point.getY());
    }

    void Rectangle::setBottomRight(const Point& point) {
        setRight(point.getX());
        setBottom(point.getY());
    }

    void Rectangle::setSize(const Point& size) {
        m_width = size.getX();
        m_height = size.getY();
        width = m_width;
        height = m_height;
    }

    void Rectangle::setTo(double x, double y, double width, double height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    void Rectangle::copyFrom(const Rectangle& other) {
        m_x = other.m_x;
        m_y = other.m_y;
        m_width = other.m_width;
        m_height = other.m_height;
        x = m_x;
        y = m_y;
        width = m_width;
        height = m_height;
    }

    Rectangle Rectangle::clone() const {
        return Rectangle(m_x, m_y, m_width, m_height);
    }

    bool Rectangle::equals(const Rectangle& other) const {
        const double epsilon = 1e-10;
        return std::abs(m_x - other.m_x) < epsilon &&
               std::abs(m_y - other.m_y) < epsilon &&
               std::abs(m_width - other.m_width) < epsilon &&
               std::abs(m_height - other.m_height) < epsilon;
    }

    bool Rectangle::isEmpty() const {
        return m_width <= 0.0 || m_height <= 0.0;
    }

    void Rectangle::setEmpty() {
        m_x = 0.0;
        m_y = 0.0;
        m_width = 0.0;
        m_height = 0.0;
    }

    void Rectangle::inflate(double dx, double dy) {
        m_x -= dx;
        m_y -= dy;
        m_width += 2.0 * dx;
        m_height += 2.0 * dy;
    }

    void Rectangle::inflatePoint(const Point& point) {
        inflate(point.getX(), point.getY());
    }

    void Rectangle::offset(double dx, double dy) {
        m_x += dx;
        m_y += dy;
    }

    void Rectangle::offsetPoint(const Point& point) {
        offset(point.getX(), point.getY());
    }

    bool Rectangle::contains(double x, double y) const {
        return x >= m_x && x < m_x + m_width && y >= m_y && y < m_y + m_height;
    }

    bool Rectangle::containsPoint(const Point& point) const {
        return contains(point.getX(), point.getY());
    }

    bool Rectangle::containsRect(const Rectangle& rect) const {
        return rect.m_x >= m_x && rect.m_y >= m_y &&
               rect.m_x + rect.m_width <= m_x + m_width &&
               rect.m_y + rect.m_height <= m_y + m_height;
    }

    bool Rectangle::intersects(const Rectangle& rect) const {
        return !(rect.m_x >= m_x + m_width || rect.m_x + rect.m_width <= m_x ||
                 rect.m_y >= m_y + m_height || rect.m_y + rect.m_height <= m_y);
    }

    Rectangle Rectangle::intersection(const Rectangle& rect) const {
        if (!intersects(rect)) {
            return Rectangle();
        }

        double left = std::max(m_x, rect.m_x);
        double top = std::max(m_y, rect.m_y);
        double right = std::min(m_x + m_width, rect.m_x + rect.m_width);
        double bottom = std::min(m_y + m_height, rect.m_y + rect.m_height);

        return Rectangle(left, top, right - left, bottom - top);
    }

    Rectangle& Rectangle::intersectsWith(const Rectangle& rect) {
        if (!intersects(rect)) {
            setEmpty();
            return *this;
        }

        double left = std::max(m_x, rect.m_x);
        double top = std::max(m_y, rect.m_y);
        double right = std::min(m_x + m_width, rect.m_x + rect.m_width);
        double bottom = std::min(m_y + m_height, rect.m_y + rect.m_height);

        setTo(left, top, right - left, bottom - top);
        return *this;
    }

    Rectangle Rectangle::unionRect(const Rectangle& rect) const {
        if (isEmpty()) {
            return rect.clone();
        }
        if (rect.isEmpty()) {
            return clone();
        }

        double left = std::min(m_x, rect.m_x);
        double top = std::min(m_y, rect.m_y);
        double right = std::max(m_x + m_width, rect.m_x + rect.m_width);
        double bottom = std::max(m_y + m_height, rect.m_y + rect.m_height);

        return Rectangle(left, top, right - left, bottom - top);
    }

    Rectangle& Rectangle::unionWith(const Rectangle& rect) {
        if (isEmpty()) {
            copyFrom(rect);
            return *this;
        }
        if (rect.isEmpty()) {
            return *this;
        }

        double left = std::min(m_x, rect.m_x);
        double top = std::min(m_y, rect.m_y);
        double right = std::max(m_x + m_width, rect.m_x + rect.m_width);
        double bottom = std::max(m_y + m_height, rect.m_y + rect.m_height);

        setTo(left, top, right - left, bottom - top);
        return *this;
    }

    Rectangle& Rectangle::unionWithPoint(const Point& point) {
        if (isEmpty()) {
            setTo(point.getX(), point.getY(), 0.0, 0.0);
            return *this;
        }

        double left = std::min(m_x, point.getX());
        double top = std::min(m_y, point.getY());
        double right = std::max(m_x + m_width, point.getX());
        double bottom = std::max(m_y + m_height, point.getY());

        setTo(left, top, right - left, bottom - top);
        return *this;
    }

    Rectangle& Rectangle::operator=(const Rectangle& other) {
        if (this != &other) {
            copyFrom(other);
        }
        return *this;
    }

    bool Rectangle::operator==(const Rectangle& other) const {
        return equals(other);
    }

    bool Rectangle::operator!=(const Rectangle& other) const {
        return !equals(other);
    }

    // 对象池实现
    std::shared_ptr<Rectangle> Rectangle::create(double x, double y, double width, double height) {
        if (s_pool.empty()) {
            s_pool.reserve(POOL_SIZE);
        }
        
        if (s_poolIndex < s_pool.size()) {
            auto rect = s_pool[s_poolIndex++];
            rect->setTo(x, y, width, height);
            return rect;
        }
        
        auto rect = std::make_shared<Rectangle>(x, y, width, height);
        if (s_pool.size() < POOL_SIZE) {
            s_pool.push_back(rect);
            s_poolIndex++;
        }
        return rect;
    }

    void Rectangle::release(std::shared_ptr<Rectangle> rect) {
        if (!rect || s_poolIndex == 0) {
            return;
        }
        
        // 重置矩形并放回池中
        rect->setEmpty();
        s_poolIndex--;
        
        // 将该矩形移到池的开头位置
        if (s_poolIndex < s_pool.size()) {
            std::swap(s_pool[s_poolIndex], s_pool.back());
        }
    }

} // namespace egret