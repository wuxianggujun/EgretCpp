#pragma once
#include "core/HashObject.hpp"
#include "geom/Point.hpp"
#include "geom/Matrix.hpp"
#include <vector>
#include <memory>
#include <string>

// Skia前向声明
class SkPath;
class SkPaint;

namespace egret {
namespace sys {

    /**
     * Path2D类用于创建和管理2D路径
     * 基于TypeScript egret.sys.Path2D实现，集成Skia路径系统
     */
    class Path2D : public HashObject {
    public:
        /**
         * 构造函数
         */
        Path2D();
        
        /**
         * 析构函数
         */
        virtual ~Path2D();

        // ========== 路径绘制方法 ==========
        
        /**
         * 移动到指定点
         */
        void moveTo(double x, double y);

        /**
         * 绘制直线到指定点
         */
        void lineTo(double x, double y);

        /**
         * 绘制二次贝塞尔曲线
         */
        void curveTo(double controlX, double controlY, double anchorX, double anchorY);

        /**
         * 绘制三次贝塞尔曲线
         */
        void cubicCurveTo(double controlX1, double controlY1, 
                         double controlX2, double controlY2,
                         double anchorX, double anchorY);

        /**
         * 绘制矩形
         */
        void drawRect(double x, double y, double width, double height);

        /**
         * 绘制圆角矩形
         */
        void drawRoundRect(double x, double y, double width, double height,
                          double ellipseWidth, double ellipseHeight);

        /**
         * 绘制圆形
         */
        void drawCircle(double x, double y, double radius);

        /**
         * 绘制椭圆
         */
        void drawEllipse(double x, double y, double width, double height);

        /**
         * 绘制圆弧
         */
        void drawArc(double x, double y, double radius, 
                    double startAngle, double endAngle, bool anticlockwise = false);

        // ========== 路径操作方法 ==========
        
        /**
         * 关闭当前路径
         */
        void closePath();

        /**
         * 清空路径
         */
        void clear();

        // ========== 填充相关方法 ==========
        
        /**
         * 开始填充
         */
        void beginFill(uint32_t color, double alpha);

        /**
         * 开始渐变填充
         */
        void beginGradientFill(const std::string& type,
                              const std::vector<uint32_t>& colors,
                              const std::vector<double>& alphas,
                              const std::vector<uint8_t>& ratios,
                              const Matrix* matrix);

        /**
         * 结束填充
         */
        void endFill();

        // ========== Skia集成方法 ==========
        
        /**
         * 获取Skia路径对象
         */
        SkPath* getSkiaPath() const { return m_skiaPath.get(); }

        /**
         * 获取填充画笔
         */
        SkPaint* getFillPaint() const { return m_fillPaint.get(); }

        /**
         * 是否有填充
         */
        bool hasFill() const { return m_hasFill; }

        /**
         * 是否有渐变填充
         */
        bool hasGradientFill() const { return m_hasGradientFill; }

        // ========== 填充规则与布尔运算 ==========
        
        /**
         * 设置是否使用偶奇填充规则（Even-Odd）。
         * 当为true时，路径中的“内部子轮廓”将产生空洞效果。
         */
        void setFillEvenOdd(bool enabled);
        bool isFillEvenOdd() const { return m_fillEvenOdd; }

        /**
         * 与另一路径进行布尔运算（原地修改当前路径）。
         * @param other 另一条路径
         * @param op 运算类型（"union"|"intersect"|"difference"|"xor"|"reverse_difference"）
         * @return 是否成功
         */
        bool booleanOp(const Path2D& other, const std::string& op);

        // ========== 状态管理 ==========
        
        /**
         * 获取当前位置X
         */
        double getLastX() const { return m_lastX; }

        /**
         * 获取当前位置Y
         */
        double getLastY() const { return m_lastY; }

        /**
         * 设置当前位置
         */
        void setLastPosition(double x, double y) { 
            m_lastX = x; 
            m_lastY = y; 
        }

        /**
         * 是否为空路径
         */
        bool isEmpty() const;

    public:
        // 内部属性，供Graphics类访问
        double m_lastX = 0.0;   // 当前X坐标（公共访问以兼容原设计）
        double m_lastY = 0.0;   // 当前Y坐标（公共访问以兼容原设计）

    private:
        // ========== 私有成员变量 ==========
        
        // Skia对象
        std::unique_ptr<SkPath> m_skiaPath;       // Skia路径对象
        std::unique_ptr<SkPaint> m_fillPaint;     // 填充画笔

        // 填充状态
        bool m_hasFill = false;                   // 是否有填充
        bool m_hasGradientFill = false;           // 是否有渐变填充
        uint32_t m_fillColor = 0;                 // 填充颜色
        double m_fillAlpha = 1.0;                 // 填充透明度

        // 渐变填充参数
        std::string m_gradientType;               // 渐变类型
        std::vector<uint32_t> m_gradientColors;   // 渐变颜色
        std::vector<double> m_gradientAlphas;     // 渐变透明度
        std::vector<uint8_t> m_gradientRatios;    // 渐变比率
        std::unique_ptr<Matrix> m_gradientMatrix; // 渐变变换矩阵

        // ========== 私有辅助方法 ==========
        
        /**
         * 更新填充画笔
         */
        void updateFillPaint();

        /**
         * 创建渐变着色器
         */
        void createGradientShader();

        // 填充规则
        bool m_fillEvenOdd = false;
    };

} // namespace sys
} // namespace egret
