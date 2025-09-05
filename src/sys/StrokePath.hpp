#pragma once
#include "sys/Path2D.hpp"
#include <vector>
#include <string>

// Skia前向声明
class SkPaint;

namespace egret {
namespace sys {

    /**
     * StrokePath类用于创建和管理线条路径
     * 基于TypeScript egret.sys.StrokePath实现，集成Skia线条绘制系统
     */
    class StrokePath : public Path2D {
    public:
        /**
         * 构造函数
         */
        StrokePath();
        
        /**
         * 析构函数
         */
        virtual ~StrokePath();

        // ========== 线条样式方法 ==========
        
        /**
         * 设置线条宽度
         */
        void setLineWidth(double width) { m_thickness = width; updateStrokePaint(); }
        
        /**
         * 设置线条颜色
         */
        void setLineColor(uint32_t color) { m_strokeColor = color; updateStrokePaint(); }
        
        /**
         * 设置线条透明度
         */
        void setLineAlpha(double alpha) { m_strokeAlpha = alpha; updateStrokePaint(); }
        
        /**
         * 设置线条样式
         * @param thickness 线条粗细
         * @param color 线条颜色
         * @param alpha 线条透明度
         * @param caps 线条端点样式
         * @param joints 线条连接样式
         * @param miterLimit 斜接限制
         * @param lineDash 虚线样式
         */
        void setLineStyle(double thickness, 
                         uint32_t color, 
                         double alpha,
                         const std::string& caps,
                         const std::string& joints,
                         double miterLimit,
                         const std::vector<double>& lineDash);

        // ========== Skia集成方法 ==========
        
        /**
         * 获取线条画笔
         */
        SkPaint* getStrokePaint() const { return m_strokePaint.get(); }

        /**
         * 是否有线条样式
         */
        bool hasStroke() const { return m_hasStroke; }

        /**
         * 获取线条粗细
         */
        double getThickness() const { return m_thickness; }

    private:
        // ========== 私有成员变量 ==========
        
        // Skia对象
        std::unique_ptr<SkPaint> m_strokePaint;     // 线条画笔

        // 线条属性
        bool m_hasStroke = false;                   // 是否有线条样式
        double m_thickness = 0.0;                   // 线条粗细
        uint32_t m_strokeColor = 0;                 // 线条颜色
        double m_strokeAlpha = 1.0;                 // 线条透明度
        std::string m_caps;                         // 端点样式
        std::string m_joints;                       // 连接样式
        double m_miterLimit = 3.0;                  // 斜接限制
        std::vector<double> m_lineDash;             // 虚线样式

        // ========== 私有辅助方法 ==========
        
        /**
         * 更新线条画笔
         */
        void updateStrokePaint();

        /**
         * 设置Skia线条端点样式
         */
        void setSkiaStrokeCap(const std::string& caps);

        /**
         * 设置Skia线条连接样式
         */
        void setSkiaStrokeJoin(const std::string& joints);

        /**
         * 设置Skia虚线效果
         */
        void setSkiaPathEffect(const std::vector<double>& lineDash);
    };

} // namespace sys
} // namespace egret