/**
 * @file Color4.hpp  
 * @brief Color4类 - 四分量颜色(RGBA)
 * 
 * 虽然原始TypeScript实现中没有Color4类，但在C++实现中需要用于
 * WebGL着色器的uniform参数存储，特别是滤镜效果中的颜色处理。
 */

#pragma once

namespace egret {

/**
 * @brief Color4类 - 四分量颜色(RGBA)
 * 
 * 用于存储颜色信息，包含红、绿、蓝、透明度四个分量。
 * 主要用于WebGL着色器的uniform参数传递。
 * 
 * @note 这是C++实现特有的类，TypeScript版本使用数字值表示颜色
 */
class Color4 {
public:
    /**
     * @brief 颜色分量
     */
    double r, g, b, a;  // 红、绿、蓝、透明度分量
    
    /**
     * @brief 默认构造函数 - 创建黑色透明颜色
     */
    Color4() : r(0.0), g(0.0), b(0.0), a(0.0) {}
    
    /**
     * @brief 构造函数
     * @param red 红色分量 (0.0 - 1.0)
     * @param green 绿色分量 (0.0 - 1.0) 
     * @param blue 蓝色分量 (0.0 - 1.0)
     * @param alpha 透明度分量 (0.0 - 1.0)
     */
    Color4(double red, double green, double blue, double alpha = 1.0)
        : r(red), g(green), b(blue), a(alpha) {}
        
    /**
     * @brief 从十六进制颜色值构造
     * @param hexColor 十六进制颜色值 (如: 0xFF0000 代表红色)
     * @param alpha 透明度分量 (0.0 - 1.0)
     */
    Color4(uint32_t hexColor, double alpha = 1.0) {
        r = ((hexColor >> 16) & 0xFF) / 255.0;
        g = ((hexColor >> 8) & 0xFF) / 255.0;  
        b = (hexColor & 0xFF) / 255.0;
        a = alpha;
    }
    
    /**
     * @brief 拷贝构造函数
     */
    Color4(const Color4& other) = default;
    
    /**
     * @brief 赋值运算符
     */
    Color4& operator=(const Color4& other) = default;
    
    /**
     * @brief 析构函数
     */
    ~Color4() = default;
    
    // ========== 操作方法 ==========
    
    /**
     * @brief 设置颜色分量
     * @param red 红色分量 (0.0 - 1.0)
     * @param green 绿色分量 (0.0 - 1.0)
     * @param blue 蓝色分量 (0.0 - 1.0) 
     * @param alpha 透明度分量 (0.0 - 1.0)
     */
    void set(double red, double green, double blue, double alpha = 1.0) {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
    
    /**
     * @brief 从十六进制颜色值设置
     * @param hexColor 十六进制颜色值 (如: 0xFF0000 代表红色)
     * @param alpha 透明度分量 (0.0 - 1.0)
     */
    void setHex(uint32_t hexColor, double alpha = 1.0) {
        r = ((hexColor >> 16) & 0xFF) / 255.0;
        g = ((hexColor >> 8) & 0xFF) / 255.0;
        b = (hexColor & 0xFF) / 255.0;
        a = alpha;
    }
    
    /**
     * @brief 转换为十六进制颜色值
     * @return 十六进制颜色值 (不包含alpha)
     */
    uint32_t toHex() const {
        uint8_t red = static_cast<uint8_t>(r * 255.0);
        uint8_t green = static_cast<uint8_t>(g * 255.0);
        uint8_t blue = static_cast<uint8_t>(b * 255.0);
        return (red << 16) | (green << 8) | blue;
    }
    
    /**
     * @brief 线性插值
     * @param other 目标颜色
     * @param t 插值参数 (0.0 - 1.0)
     * @return 插值后的颜色
     */
    Color4 lerp(const Color4& other, double t) const {
        return Color4(
            r + (other.r - r) * t,
            g + (other.g - g) * t,
            b + (other.b - b) * t,
            a + (other.a - a) * t
        );
    }
    
    // ========== 运算符重载 ==========
    
    /**
     * @brief 相等比较
     */
    bool operator==(const Color4& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
    
    /**
     * @brief 不等比较
     */
    bool operator!=(const Color4& other) const {
        return !(*this == other);
    }
    
    /**
     * @brief 颜色加法
     */
    Color4 operator+(const Color4& other) const {
        return Color4(r + other.r, g + other.g, b + other.b, a + other.a);
    }
    
    /**
     * @brief 颜色减法
     */
    Color4 operator-(const Color4& other) const {
        return Color4(r - other.r, g - other.g, b - other.b, a - other.a);
    }
    
    /**
     * @brief 颜色乘法（标量）
     */
    Color4 operator*(double scalar) const {
        return Color4(r * scalar, g * scalar, b * scalar, a * scalar);
    }
    
    // ========== 预定义颜色常量 ==========
    
    static const Color4 BLACK;      ///< 黑色 (0, 0, 0, 1)
    static const Color4 WHITE;      ///< 白色 (1, 1, 1, 1)  
    static const Color4 RED;        ///< 红色 (1, 0, 0, 1)
    static const Color4 GREEN;      ///< 绿色 (0, 1, 0, 1)
    static const Color4 BLUE;       ///< 蓝色 (0, 0, 1, 1)
    static const Color4 TRANSPARENT; ///< 透明 (0, 0, 0, 0)
};

} // namespace egret