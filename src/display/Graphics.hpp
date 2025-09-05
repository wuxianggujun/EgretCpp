#pragma once
#include "core/HashObject.hpp"
#include "geom/Point.hpp"
#include "geom/Matrix.hpp"
#include "geom/Rectangle.hpp"
#include "player/RenderNode.hpp"
#include "display/LineStyles.hpp"
#include <string>
#include <vector>
#include <memory>

// Skia前向声明
class SkPath;
class SkPaint;

namespace egret {
    
    // 前向声明
    class DisplayObject;
    class Sprite;
    
    namespace sys {
        class GraphicsNode;
        class Path2D;
        class StrokePath;
    }

    /**
     * 渐变类型常量
     */
    class GradientType {
    public:
        static const std::string LINEAR;   // "linear"
        static const std::string RADIAL;   // "radial"
    };

    /**
     * Graphics类包含一组可用来创建矢量形状的方法
     * 支持绘制的显示对象包括Sprite和Shape对象
     * 基于TypeScript egret.Graphics完整翻译，集成Skia渲染后端
     * 
     * @version Egret 2.4
     * @platform Web,Native
     */
    class Graphics : public HashObject {
    public:
        /**
         * 创建一个Graphics对象
         */
        Graphics();
        
        /**
         * 析构函数
         */
        virtual ~Graphics();

        // ========== 填充样式方法 ==========
        
        /**
         * 指定一种简单的单一颜色填充，在绘制时该填充将在随后对其他Graphics方法的调用中使用
         * 调用clear()方法会清除填充
         * @param color 填充的颜色
         * @param alpha 填充的Alpha值 (0.0 - 1.0)
         */
        void beginFill(uint32_t color, double alpha = 1.0);

        /**
         * 指定一种渐变填充，用于随后调用对象的其他Graphics方法
         * 调用clear()方法会清除填充
         * @param type 渐变类型：GradientType::LINEAR 或 GradientType::RADIAL
         * @param colors 渐变中使用的RGB十六进制颜色值的数组
         * @param alphas colors数组中对应颜色的alpha值数组
         * @param ratios 颜色分布比率的数组，有效值为0到255
         * @param matrix 转换矩阵
         */
        void beginGradientFill(const std::string& type, 
                              const std::vector<uint32_t>& colors,
                              const std::vector<double>& alphas, 
                              const std::vector<uint8_t>& ratios,
                              const Matrix* matrix = nullptr);

        /**
         * 对从上一次调用beginFill()方法之后添加的直线和曲线应用填充
         */
        void endFill();

        // ========== 线条样式方法 ==========
        
        /**
         * 指定一种线条样式以用于随后对lineTo()或drawCircle()等Graphics方法的调用
         * @param thickness 线条的粗细，有效值为0到255
         * @param color 线条的十六进制颜色值
         * @param alpha 表示线条颜色的Alpha值
         * @param pixelHinting 布尔型值，指定是否提示笔触采用完整像素
         * @param scaleMode 用于指定要使用的比例模式
         * @param caps 用于指定线条末端处端点类型
         * @param joints 指定用于拐角的连接外观的类型
         * @param miterLimit 用于表示剪切斜接的极限值的数字
         * @param lineDash 设置虚线样式
         */
        void lineStyle(double thickness = NAN, 
                      uint32_t color = 0, 
                      double alpha = 1.0,
                      bool pixelHinting = false,
                      const std::string& scaleMode = "normal",
                      const std::string& caps = "",
                      const std::string& joints = "",
                      double miterLimit = 3.0,
                      const std::vector<double>& lineDash = {});

        // ========== 基本图形绘制方法 ==========
        
        /**
         * 绘制一个矩形
         * @param x 矩形左上角的x位置
         * @param y 矩形左上角的y位置
         * @param width 矩形的宽度
         * @param height 矩形的高度
         */
        void drawRect(double x, double y, double width, double height);

        /**
         * 绘制一个圆角矩形
         * @param x 矩形左上角的x位置
         * @param y 矩形左上角的y位置
         * @param width 矩形的宽度
         * @param height 矩形的高度
         * @param ellipseWidth 用于绘制圆角的椭圆的宽度
         * @param ellipseHeight 用于绘制圆角的椭圆的高度
         */
        void drawRoundRect(double x, double y, double width, double height,
                          double ellipseWidth, double ellipseHeight = NAN);

        /**
         * 绘制一个圆
         * @param x 圆心的x位置
         * @param y 圆心的y位置
         * @param radius 圆的半径
         */
        void drawCircle(double x, double y, double radius);

        /**
         * 绘制一个椭圆
         * @param x 椭圆外接矩形左上角的x位置
         * @param y 椭圆外接矩形左上角的y位置
         * @param width 椭圆外接矩形的宽度
         * @param height 椭圆外接矩形的高度
         */
        void drawEllipse(double x, double y, double width, double height);

        // ========== 路径绘制方法 ==========
        
        /**
         * 将当前绘图位置移动到(x, y)
         * @param x 目标x坐标
         * @param y 目标y坐标
         */
        void moveTo(double x, double y);

        /**
         * 使用当前线条样式绘制一条从当前绘图位置开始到(x, y)结束的直线
         * @param x 目标x坐标
         * @param y 目标y坐标
         */
        void lineTo(double x, double y);

        /**
         * 使用当前线条样式绘制一条二次贝塞尔曲线
         * @param controlX 控制点的x坐标
         * @param controlY 控制点的y坐标
         * @param anchorX 锚点的x坐标
         * @param anchorY 锚点的y坐标
         */
        void curveTo(double controlX, double controlY, double anchorX, double anchorY);

        /**
         * 绘制一条三次贝塞尔曲线
         * @param controlX1 第一个控制点的x坐标
         * @param controlY1 第一个控制点的y坐标
         * @param controlX2 第二个控制点的x坐标
         * @param controlY2 第二个控制点的y坐标
         * @param anchorX 锚点的x坐标
         * @param anchorY 锚点的y坐标
         */
        void cubicCurveTo(double controlX1, double controlY1, 
                         double controlX2, double controlY2,
                         double anchorX, double anchorY);

        /**
         * 绘制一段圆弧路径
         * @param x 圆弧中心的x轴坐标
         * @param y 圆弧中心的y轴坐标
         * @param radius 圆弧的半径
         * @param startAngle 圆弧的起始点，x轴方向开始计算，单位以弧度表示
         * @param endAngle 圆弧的终点，单位以弧度表示
         * @param anticlockwise 如果为true，逆时针绘制圆弧，反之，顺时针绘制
         */
        void drawArc(double x, double y, double radius, 
                    double startAngle, double endAngle, 
                    bool anticlockwise = false);

        // ========== 清除和控制方法 ==========
        
        /**
         * 清除绘制到此Graphics对象的图形，并重置填充和线条样式设置
         */
        void clear();

        // ========== 边界和测量方法 ==========
        
        /**
         * 测量内容边界
         * @param bounds 输出边界矩形
         */
        void measureContentBounds(Rectangle& bounds);

        /**
         * 点击测试
         * @param stageX 舞台x坐标
         * @param stageY 舞台y坐标
         * @return 如果点击命中返回目标显示对象，否则返回nullptr
         */
        DisplayObject* hitTest(double stageX, double stageY);

        // ========== 内部系统方法 ==========
        
        /**
         * 设置绑定到的目标显示对象（内部使用）
         * @param target 目标显示对象
         */
        void setTarget(DisplayObject* target);

        /**
         * 获取渲染节点（内部使用）
         */
        std::shared_ptr<sys::GraphicsNode> getRenderNode() const { return m_renderNode; }

        /**
         * 当从舞台移除时调用（内部使用）
         */
        void onRemoveFromStage();

    private:
        // ========== 私有辅助方法 ==========
        
        /**
         * 标记渲染脏
         */
        void dirty();

        /**
         * 设置线条宽度（处理1像素和3像素的特殊情况）
         */
        void setStrokeWidth(double width);

        /**
         * 扩展边界（通过点）
         */
        void extendBoundsByPoint(double x, double y);

        /**
         * 扩展边界（通过X坐标）
         */
        void extendBoundsByX(double x);

        /**
         * 扩展边界（通过Y坐标）
         */
        void extendBoundsByY(double y);

        /**
         * 更新节点边界
         */
        void updateNodeBounds();

        /**
         * 更新当前位置
         */
        void updatePosition(double x, double y);

        /**
         * 测量圆弧的矩形大小
         */
        void arcBounds(double x, double y, double radius, double startAngle, double endAngle);

        /**
         * 格式化弧线角度的值
         */
        static double clampAngle(double value);

        /**
         * 创建贝塞尔曲线点
         */
        static std::vector<Point> createBezierPoints(const std::vector<double>& pointsData, int pointsAmount);

        /**
         * 根据锚点组与取值系数获取贝塞尔曲线上的一点
         */
        static Point getBezierPointByFactor(const std::vector<double>& pointsData, double t);

        /**
         * 通过factor参数获取二次贝塞尔曲线上的位置
         */
        static double getCurvePoint(double value0, double value1, double value2, double factor);

        /**
         * 通过factor参数获取三次贝塞尔曲线上的位置
         */
        static double getCubicCurvePoint(double value0, double value1, double value2, double value3, double factor);

        // ========== 私有成员变量 ==========
        
        // 渲染系统
        std::shared_ptr<sys::GraphicsNode> m_renderNode;  // 渲染节点
        DisplayObject* m_targetDisplay = nullptr;         // 绑定的目标显示对象
        bool m_targetIsSprite = false;                    // 目标是否为Sprite

        // 当前绘图状态
        double m_lastX = 0.0;                             // 当前x坐标
        double m_lastY = 0.0;                             // 当前y坐标
        std::shared_ptr<sys::Path2D> m_fillPath;          // 当前正在绘制的填充路径
        std::shared_ptr<sys::StrokePath> m_strokePath;    // 当前正在绘制的线条路径

        // 线条宽度处理
        double m_topLeftStrokeWidth = 0.0;                // 线条左上方宽度
        double m_bottomRightStrokeWidth = 0.0;            // 线条右下方宽度

        // 边界计算
        double m_minX = INFINITY;                         // 最小X边界
        double m_minY = INFINITY;                         // 最小Y边界  
        double m_maxX = -INFINITY;                        // 最大X边界
        double m_maxY = -INFINITY;                        // 最大Y边界
        bool m_includeLastPosition = true;                // 是否已经包含上一次moveTo的坐标点
    };

} // namespace egret