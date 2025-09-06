#include "display/Graphics.hpp"
#include "display/DisplayObject.hpp"
#include "display/Sprite.hpp"
#include "display/LineStyles.hpp"
#include "sys/GraphicsNode.hpp"
#include "sys/Path2D.hpp"
#include "sys/StrokePath.hpp"
#include "utils/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace egret {

    // ========== 静态常量定义 ==========
    
    const std::string GradientType::LINEAR = "linear";
    const std::string GradientType::RADIAL = "radial";

    // ========== Graphics类实现 ==========
    
    Graphics::Graphics() {
        EGRET_DEBUG("Graphics::Graphics() - Creating Graphics object");
        // 创建GraphicsNode渲染节点
        m_renderNode = std::make_shared<sys::GraphicsNode>();
        EGRET_DEBUG("Graphics::Graphics() - GraphicsNode created successfully");
    }

    Graphics::~Graphics() {
        // 清理资源
        clear();
    }

    // ========== 填充样式方法 ==========
    
    void Graphics::beginFill(uint32_t color, double alpha) {
        EGRET_DEBUGF("Graphics::beginFill() - color=0x{:X}, alpha={}", color, alpha);
        
        color = color & 0xFFFFFF;  // 确保颜色在有效范围内
        alpha = std::clamp(alpha, 0.0, 1.0);  // 限制alpha范围
        
        // 通过GraphicsNode创建填充路径（StrokePath转换为Path2D基类指针）
        m_fillPath = m_renderNode->beginFill(color, alpha, 
                                           std::static_pointer_cast<sys::Path2D>(m_strokePath));
        
        if (m_fillPath) {
            EGRET_DEBUG("Graphics::beginFill() - Fill path created successfully");
        } else {
            EGRET_WARN("Graphics::beginFill() - Failed to create fill path");
        }
        
        // 如果已有绘制数据，将当前位置移动到填充路径
        if (!m_renderNode->getDrawData().empty()) {
            EGRET_DEBUGF("Graphics::beginFill() - Moving to current position ({}, {})", m_lastX, m_lastY);
            m_fillPath->moveTo(m_lastX, m_lastY);
        }
    }

    void Graphics::beginGradientFill(const std::string& type, 
                                    const std::vector<uint32_t>& colors,
                                    const std::vector<double>& alphas, 
                                    const std::vector<uint8_t>& ratios,
                                    const Matrix* matrix) {
        // 将uint8_t的ratios转换为double
        std::vector<double> convertedRatios;
        convertedRatios.reserve(ratios.size());
        for (uint8_t ratio : ratios) {
            convertedRatios.push_back(static_cast<double>(ratio));
        }
        
        // 通过GraphicsNode创建渐变填充路径
        m_fillPath = m_renderNode->beginGradientFill(type, colors, alphas, convertedRatios, matrix, 
                                                   std::static_pointer_cast<sys::Path2D>(m_strokePath));
        
        // 如果已有绘制数据，将当前位置移动到填充路径
        if (!m_renderNode->getDrawData().empty()) {
            m_fillPath->moveTo(m_lastX, m_lastY);
        }
    }

    void Graphics::endFill() {
        m_fillPath.reset();
    }

    // ========== 线条样式方法 ==========
    
    void Graphics::lineStyle(double thickness, uint32_t color, double alpha,
                            bool pixelHinting, const std::string& scaleMode,
                            const std::string& caps, const std::string& joints,
                            double miterLimit, const std::vector<double>& lineDash) {
        thickness = std::max(0.0, thickness);  // 确保粗细非负
        color = color & 0xFFFFFF;              // 确保颜色在有效范围内
        alpha = std::clamp(alpha, 0.0, 1.0);   // 限制alpha范围
        miterLimit = std::max(0.0, miterLimit); // 确保miterLimit非负
        
        if (thickness <= 0) {
            // 如果线条粗细为0或负数，清除线条路径
            m_strokePath.reset();
            setStrokeWidth(0);
        } else {
            // 设置线条宽度（用于边界计算）
            setStrokeWidth(thickness);
            
            // 处理线条端点样式
            std::string actualCaps = caps.empty() ? CapsStyle::ROUND : caps;
            std::string actualJoints = joints.empty() ? JointStyle::ROUND : joints;
            
            // 通过GraphicsNode创建线条路径
            m_strokePath = m_renderNode->lineStyle(thickness, color, alpha, 
                                                  actualCaps, actualJoints, miterLimit, lineDash);
            
            // 如果已有绘制数据，将当前位置移动到线条路径
            if (m_renderNode->getDrawData().size() > 1) {
                m_strokePath->moveTo(m_lastX, m_lastY);
            }
        }
    }

    // ========== 基本图形绘制方法 ==========
    
    void Graphics::drawRect(double x, double y, double width, double height) {
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        if (std::isnan(width)) width = 0;
        if (std::isnan(height)) height = 0;
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->drawRect(x, y, width, height);
        }
        if (m_strokePath) {
            m_strokePath->drawRect(x, y, width, height);
        }
        
        // 扩展边界
        extendBoundsByPoint(x + width, y + height);
        updatePosition(x, y);
        dirty();
    }

    void Graphics::drawRoundRect(double x, double y, double width, double height,
                                double ellipseWidth, double ellipseHeight) {
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        if (std::isnan(width)) width = 0;
        if (std::isnan(height)) height = 0;
        if (std::isnan(ellipseWidth)) ellipseWidth = 0;
        ellipseHeight = std::isnan(ellipseHeight) ? ellipseWidth : ellipseHeight;
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->drawRoundRect(x, y, width, height, ellipseWidth, ellipseHeight);
        }
        if (m_strokePath) {
            m_strokePath->drawRoundRect(x, y, width, height, ellipseWidth, ellipseHeight);
        }
        
        // 计算边界（考虑圆角）
        double radiusX = static_cast<int>(ellipseWidth * 0.5);
        double radiusY = ellipseHeight ? static_cast<int>(ellipseHeight * 0.5) : radiusX;
        double right = x + width;
        double bottom = y + height;
        double ybw = bottom - radiusY;
        
        extendBoundsByPoint(x, y);
        extendBoundsByPoint(right, bottom);
        updatePosition(right, ybw);
        dirty();
    }

    void Graphics::drawCircle(double x, double y, double radius) {
        EGRET_DEBUGF("Graphics::drawCircle() - x={}, y={}, radius={}", x, y, radius);
        
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        if (std::isnan(radius)) radius = 0;
        
        EGRET_DEBUGF("Graphics::drawCircle() - After NaN check: x={}, y={}, radius={}", x, y, radius);
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            EGRET_DEBUG("Graphics::drawCircle() - Drawing to fill path");
            m_fillPath->drawCircle(x, y, radius);
        } else {
            EGRET_WARN("Graphics::drawCircle() - No fill path available");
        }
        
        if (m_strokePath) {
            EGRET_DEBUG("Graphics::drawCircle() - Drawing to stroke path");
            m_strokePath->drawCircle(x, y, radius);
        } else {
            EGRET_DEBUG("Graphics::drawCircle() - No stroke path (normal for filled shapes)");
        }
        
        // 扩展边界（+/-1 +2 解决WebGL裁切问题）
        extendBoundsByPoint(x - radius - 1, y - radius - 1);
        extendBoundsByPoint(x + radius + 2, y + radius + 2);
        updatePosition(x + radius, y);
        
        EGRET_DEBUG("Graphics::drawCircle() - Calling dirty() to mark for redraw");
        dirty();
        
        // 检查GraphicsNode的绘制数据
        if (m_renderNode) {
            const auto& drawData = m_renderNode->getDrawData();
            EGRET_DEBUGF("Graphics::drawCircle() - GraphicsNode now has {} draw data entries", drawData.size());
        }
        
        EGRET_DEBUG("Graphics::drawCircle() - Circle drawing completed");
    }

    void Graphics::drawEllipse(double x, double y, double width, double height) {
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        if (std::isnan(width)) width = 0;
        if (std::isnan(height)) height = 0;
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->drawEllipse(x, y, width, height);
        }
        if (m_strokePath) {
            m_strokePath->drawEllipse(x, y, width, height);
        }
        
        // 扩展边界（+/-1 +2 解决WebGL裁切问题）
        extendBoundsByPoint(x - 1, y - 1);
        extendBoundsByPoint(x + width + 2, y + height + 2);
        updatePosition(x + width, y + height * 0.5);
        dirty();
    }

    // ========== 路径绘制方法 ==========
    
    void Graphics::moveTo(double x, double y) {
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        
        // 移动填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->moveTo(x, y);
        }
        if (m_strokePath) {
            m_strokePath->moveTo(x, y);
        }
        
        m_includeLastPosition = false;
        m_lastX = x;
        m_lastY = y;
        dirty();
    }

    void Graphics::lineTo(double x, double y) {
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->lineTo(x, y);
        }
        if (m_strokePath) {
            m_strokePath->lineTo(x, y);
        }
        
        updatePosition(x, y);
        dirty();
    }

    void Graphics::curveTo(double controlX, double controlY, double anchorX, double anchorY) {
        // 处理NaN和默认值
        if (std::isnan(controlX)) controlX = 0;
        if (std::isnan(controlY)) controlY = 0;
        if (std::isnan(anchorX)) anchorX = 0;
        if (std::isnan(anchorY)) anchorY = 0;
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->curveTo(controlX, controlY, anchorX, anchorY);
        }
        if (m_strokePath) {
            m_strokePath->curveTo(controlX, controlY, anchorX, anchorY);
        }
        
        // 计算贝塞尔曲线边界
        double lastX = m_lastX;
        double lastY = m_lastY;
        std::vector<double> pointsData = {lastX, lastY, controlX, controlY, anchorX, anchorY};
        auto bezierPoints = createBezierPoints(pointsData, 50);
        
        for (const auto& point : bezierPoints) {
            extendBoundsByPoint(point.getX(), point.getY());
        }
        
        extendBoundsByPoint(anchorX, anchorY);
        updatePosition(anchorX, anchorY);
        dirty();
    }

    void Graphics::cubicCurveTo(double controlX1, double controlY1, 
                               double controlX2, double controlY2,
                               double anchorX, double anchorY) {
        // 处理NaN和默认值
        if (std::isnan(controlX1)) controlX1 = 0;
        if (std::isnan(controlY1)) controlY1 = 0;
        if (std::isnan(controlX2)) controlX2 = 0;
        if (std::isnan(controlY2)) controlY2 = 0;
        if (std::isnan(anchorX)) anchorX = 0;
        if (std::isnan(anchorY)) anchorY = 0;
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->cubicCurveTo(controlX1, controlY1, controlX2, controlY2, anchorX, anchorY);
        }
        if (m_strokePath) {
            m_strokePath->cubicCurveTo(controlX1, controlY1, controlX2, controlY2, anchorX, anchorY);
        }
        
        // 计算三次贝塞尔曲线边界
        double lastX = m_lastX;
        double lastY = m_lastY;
        std::vector<double> pointsData = {lastX, lastY, controlX1, controlY1, controlX2, controlY2, anchorX, anchorY};
        auto bezierPoints = createBezierPoints(pointsData, 50);
        
        for (const auto& point : bezierPoints) {
            extendBoundsByPoint(point.getX(), point.getY());
        }
        
        extendBoundsByPoint(anchorX, anchorY);
        updatePosition(anchorX, anchorY);
        dirty();
    }

    void Graphics::drawArc(double x, double y, double radius, 
                          double startAngle, double endAngle, bool anticlockwise) {
        if (radius < 0 || startAngle == endAngle) {
            return;
        }
        
        // 处理NaN和默认值
        if (std::isnan(x)) x = 0;
        if (std::isnan(y)) y = 0;
        if (std::isnan(radius)) radius = 0;
        if (std::isnan(startAngle)) startAngle = 0;
        if (std::isnan(endAngle)) endAngle = 0;
        
        // 格式化角度
        startAngle = clampAngle(startAngle);
        endAngle = clampAngle(endAngle);
        
        // 绘制到填充路径和线条路径
        if (m_fillPath) {
            m_fillPath->m_lastX = m_lastX;
            m_fillPath->m_lastY = m_lastY;
            m_fillPath->drawArc(x, y, radius, startAngle, endAngle, anticlockwise);
        }
        if (m_strokePath) {
            m_strokePath->m_lastX = m_lastX;
            m_strokePath->m_lastY = m_lastY;
            m_strokePath->drawArc(x, y, radius, startAngle, endAngle, anticlockwise);
        }
        
        // 计算圆弧边界
        if (anticlockwise) {
            arcBounds(x, y, radius, endAngle, startAngle);
        } else {
            arcBounds(x, y, radius, startAngle, endAngle);
        }
        
        // 更新位置到圆弧终点
        double endX = x + std::cos(endAngle) * radius;
        double endY = y + std::sin(endAngle) * radius;
        updatePosition(endX, endY);
        dirty();
    }

    // ========== 清除和控制方法 ==========
    
    void Graphics::clear() {
        // 清除渲染节点
        if (m_renderNode) {
            m_renderNode->clear();
        }
        
        // 重置状态
        updatePosition(0, 0);
        m_minX = INFINITY;
        m_minY = INFINITY;
        m_maxX = -INFINITY;
        m_maxY = -INFINITY;
        
        // 清除路径引用
        m_fillPath.reset();
        m_strokePath.reset();
        
        dirty();
    }

    // ========== 边界和测量方法 ==========
    
    void Graphics::measureContentBounds(Rectangle& bounds) {
        if (m_minX == INFINITY) {
            bounds.setTo(0, 0, 0, 0);
        } else {
            bounds.setTo(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY);
        }
    }

    DisplayObject* Graphics::hitTest(double stageX, double stageY) {
        if (!m_targetDisplay) {
            return nullptr;
        }
        
        // TODO: 实现基于Skia的精确点击测试
        // 这里需要使用Skia的路径点击测试功能
        // 暂时返回基本的边界框测试结果
        return m_targetDisplay;
    }

    // ========== 内部系统方法 ==========
    
    void Graphics::setTarget(DisplayObject* target) {
        EGRET_DEBUGF("Graphics::setTarget() - Setting target to DisplayObject at address: {}", 
                    target ? "valid" : "null");
        
        // 清除旧绑定
        if (m_targetDisplay) {
            EGRET_DEBUG("Graphics::setTarget() - Clearing old target binding");
            // 明确指定RenderNode类型以避免重载二义性
            std::shared_ptr<sys::RenderNode> nullNode = nullptr;
            m_targetDisplay->setRenderNode(nullNode);
        }
        
        // 核心修复：直接赋值GraphicsNode到目标对象
        if (target) {
            EGRET_DEBUG("Graphics::setTarget() - Assigning GraphicsNode to target");
            target->setRenderNode(m_renderNode);
            
            // 检查GraphicsNode是否有绘制数据
            if (m_renderNode) {
                const auto& drawData = m_renderNode->getDrawData();
                EGRET_DEBUGF("Graphics::setTarget() - GraphicsNode has {} draw data entries", drawData.size());
            } else {
                EGRET_WARN("Graphics::setTarget() - m_renderNode is null!");
            }
        }
        
        // 建立双向引用
        m_targetDisplay = target;
        m_targetIsSprite = dynamic_cast<Sprite*>(target) != nullptr;
        
        EGRET_DEBUGF("Graphics::setTarget() - Target is Sprite: {}", m_targetIsSprite);
        
        // 标记需要重绘
        dirty();
        
        EGRET_DEBUG("Graphics::setTarget() - Target assignment completed");
    }

    void Graphics::onRemoveFromStage() {
        if (m_renderNode) {
            m_renderNode->clean();
        }
    }

    // ========== 私有辅助方法 ==========
    
    void Graphics::dirty() {
        if (m_renderNode) {
            m_renderNode->dirtyRender = true;
        }
        
        // 标记目标显示对象的缓存为脏
        if (m_targetDisplay) {
            m_targetDisplay->setCacheDirty(true);
            m_targetDisplay->cacheDirtyUp();
        }
    }

    void Graphics::setStrokeWidth(double width) {
        switch (static_cast<int>(width)) {
            case 1:
                m_topLeftStrokeWidth = 0;
                m_bottomRightStrokeWidth = 1;
                break;
            case 3:
                m_topLeftStrokeWidth = 1;
                m_bottomRightStrokeWidth = 2;
                break;
            default:
                double half = std::ceil(width * 0.5);
                m_topLeftStrokeWidth = half;
                m_bottomRightStrokeWidth = half;
                break;
        }
    }

    void Graphics::extendBoundsByPoint(double x, double y) {
        extendBoundsByX(x);
        extendBoundsByY(y);
    }

    void Graphics::extendBoundsByX(double x) {
        m_minX = std::min(m_minX, x - m_topLeftStrokeWidth);
        m_maxX = std::max(m_maxX, x + m_bottomRightStrokeWidth);
        updateNodeBounds();
    }

    void Graphics::extendBoundsByY(double y) {
        m_minY = std::min(m_minY, y - m_topLeftStrokeWidth);
        m_maxY = std::max(m_maxY, y + m_bottomRightStrokeWidth);
        updateNodeBounds();
    }

    void Graphics::updateNodeBounds() {
        if (m_renderNode) {
            m_renderNode->x = m_minX;
            m_renderNode->y = m_minY;
            m_renderNode->width = std::ceil(m_maxX - m_minX);
            m_renderNode->height = std::ceil(m_maxY - m_minY);
        }
    }

    void Graphics::updatePosition(double x, double y) {
        if (!m_includeLastPosition) {
            extendBoundsByPoint(m_lastX, m_lastY);
            m_includeLastPosition = true;
        }
        m_lastX = x;
        m_lastY = y;
        extendBoundsByPoint(x, y);
    }

    void Graphics::arcBounds(double x, double y, double radius, double startAngle, double endAngle) {
        const double PI = M_PI;
        
        if (std::abs(startAngle - endAngle) < 0.01) {
            extendBoundsByPoint(x - radius, y - radius);
            extendBoundsByPoint(x + radius, y + radius);
            return;
        }
        
        if (startAngle > endAngle) {
            endAngle += PI * 2;
        }
        
        double startX = std::cos(startAngle) * radius;
        double endX = std::cos(endAngle) * radius;
        double xMin = std::min(startX, endX);
        double xMax = std::max(startX, endX);
        
        double startY = std::sin(startAngle) * radius;
        double endY = std::sin(endAngle) * radius;
        double yMin = std::min(startY, endY);
        double yMax = std::max(startY, endY);
        
        double startRange = startAngle / (PI * 0.5);
        double endRange = endAngle / (PI * 0.5);
        
        for (int i = static_cast<int>(std::ceil(startRange)); i <= endRange; i++) {
            switch (i % 4) {
                case 0:
                    xMax = radius;
                    break;
                case 1:
                    yMax = radius;
                    break;
                case 2:
                    xMin = -radius;
                    break;
                case 3:
                    yMin = -radius;
                    break;
            }
        }
        
        xMin = std::floor(xMin);
        yMin = std::floor(yMin);
        xMax = std::ceil(xMax);
        yMax = std::ceil(yMax);
        
        extendBoundsByPoint(xMin + x, yMin + y);
        extendBoundsByPoint(xMax + x, yMax + y);
    }

    // ========== 静态辅助函数 ==========
    
    double Graphics::clampAngle(double value) {
        value = std::fmod(value, M_PI * 2);
        if (value < 0) {
            value += M_PI * 2;
        }
        return value;
    }

    std::vector<Point> Graphics::createBezierPoints(const std::vector<double>& pointsData, int pointsAmount) {
        std::vector<Point> points;
        for (int i = 0; i < pointsAmount; i++) {
            Point point = getBezierPointByFactor(pointsData, static_cast<double>(i) / pointsAmount);
            points.push_back(point);
        }
        return points;
    }

    Point Graphics::getBezierPointByFactor(const std::vector<double>& pointsData, double t) {
        size_t len = pointsData.size();
        double x = 0, y = 0;
        
        if (len / 2 == 3) {
            // 二次贝塞尔曲线
            double x0 = pointsData[0];
            double y0 = pointsData[1];
            double x1 = pointsData[2];
            double y1 = pointsData[3];
            double x2 = pointsData[4];
            double y2 = pointsData[5];
            x = getCurvePoint(x0, x1, x2, t);
            y = getCurvePoint(y0, y1, y2, t);
        } else if (len / 2 == 4) {
            // 三次贝塞尔曲线
            double x0 = pointsData[0];
            double y0 = pointsData[1];
            double x1 = pointsData[2];
            double y1 = pointsData[3];
            double x2 = pointsData[4];
            double y2 = pointsData[5];
            double x3 = pointsData[6];
            double y3 = pointsData[7];
            x = getCubicCurvePoint(x0, x1, x2, x3, t);
            y = getCubicCurvePoint(y0, y1, y2, y3, t);
        }
        
        return Point(x, y);
    }

    double Graphics::getCurvePoint(double value0, double value1, double value2, double factor) {
        return std::pow(1 - factor, 2) * value0 + 
               2 * factor * (1 - factor) * value1 + 
               std::pow(factor, 2) * value2;
    }

    double Graphics::getCubicCurvePoint(double value0, double value1, double value2, double value3, double factor) {
        return std::pow(1 - factor, 3) * value0 + 
               3 * factor * std::pow(1 - factor, 2) * value1 + 
               3 * (1 - factor) * std::pow(factor, 2) * value2 + 
               std::pow(factor, 3) * value3;
    }

} // namespace egret