#include "sys/Path2D.hpp"
#include "geom/Matrix.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Skia includes
#include <include/core/SkPath.h>
#include <include/core/SkPaint.h>
#include <include/core/SkPathTypes.h>
#include <include/pathops/SkPathOps.h>
#include <include/effects/SkGradientShader.h>
#include <include/core/SkMatrix.h>

namespace egret {
namespace sys {

    Path2D::Path2D() {
        // 创建Skia路径对象
        m_skiaPath = std::make_unique<SkPath>();
        m_fillPaint = std::make_unique<SkPaint>();
        
        // 初始化画笔
        m_fillPaint->setAntiAlias(true);
        m_fillPaint->setStyle(SkPaint::kFill_Style);
    }

    Path2D::~Path2D() {
        // 智能指针会自动释放资源
    }

    // ========== 路径绘制方法 ==========
    
    void Path2D::moveTo(double x, double y) {
        m_skiaPath->moveTo(static_cast<SkScalar>(x), static_cast<SkScalar>(y));
        m_lastX = x;
        m_lastY = y;
    }

    void Path2D::lineTo(double x, double y) {
        m_skiaPath->lineTo(static_cast<SkScalar>(x), static_cast<SkScalar>(y));
        m_lastX = x;
        m_lastY = y;
    }

    void Path2D::curveTo(double controlX, double controlY, double anchorX, double anchorY) {
        m_skiaPath->quadTo(
            static_cast<SkScalar>(controlX), static_cast<SkScalar>(controlY),
            static_cast<SkScalar>(anchorX), static_cast<SkScalar>(anchorY)
        );
        m_lastX = anchorX;
        m_lastY = anchorY;
    }

    void Path2D::cubicCurveTo(double controlX1, double controlY1, 
                             double controlX2, double controlY2,
                             double anchorX, double anchorY) {
        m_skiaPath->cubicTo(
            static_cast<SkScalar>(controlX1), static_cast<SkScalar>(controlY1),
            static_cast<SkScalar>(controlX2), static_cast<SkScalar>(controlY2),
            static_cast<SkScalar>(anchorX), static_cast<SkScalar>(anchorY)
        );
        m_lastX = anchorX;
        m_lastY = anchorY;
    }

    void Path2D::drawRect(double x, double y, double width, double height) {
        SkRect rect = SkRect::MakeXYWH(
            static_cast<SkScalar>(x), static_cast<SkScalar>(y),
            static_cast<SkScalar>(width), static_cast<SkScalar>(height)
        );
        m_skiaPath->addRect(rect);
        m_lastX = x;
        m_lastY = y;
    }

    void Path2D::drawRoundRect(double x, double y, double width, double height,
                              double ellipseWidth, double ellipseHeight) {
        SkRect rect = SkRect::MakeXYWH(
            static_cast<SkScalar>(x), static_cast<SkScalar>(y),
            static_cast<SkScalar>(width), static_cast<SkScalar>(height)
        );
        
        SkScalar radiusX = static_cast<SkScalar>(ellipseWidth * 0.5);
        SkScalar radiusY = static_cast<SkScalar>(ellipseHeight * 0.5);
        
        m_skiaPath->addRoundRect(rect, radiusX, radiusY);
        m_lastX = x;
        m_lastY = y;
    }

    void Path2D::drawCircle(double x, double y, double radius) {
        m_skiaPath->addCircle(
            static_cast<SkScalar>(x), static_cast<SkScalar>(y),
            static_cast<SkScalar>(radius)
        );
        m_lastX = x + radius;
        m_lastY = y;
    }

    void Path2D::drawEllipse(double x, double y, double width, double height) {
        SkRect rect = SkRect::MakeXYWH(
            static_cast<SkScalar>(x), static_cast<SkScalar>(y),
            static_cast<SkScalar>(width), static_cast<SkScalar>(height)
        );
        m_skiaPath->addOval(rect);
        m_lastX = x + width;
        m_lastY = y + height * 0.5;
    }

    void Path2D::drawArc(double x, double y, double radius, 
                        double startAngle, double endAngle, bool anticlockwise) {
        // 将弧度转换为度数
        SkScalar startDegrees = static_cast<SkScalar>(startAngle * 180.0 / M_PI);
        SkScalar endDegrees = static_cast<SkScalar>(endAngle * 180.0 / M_PI);
        
        // 计算扫描角度
        SkScalar sweepAngle = endDegrees - startDegrees;
        if (anticlockwise) {
            if (sweepAngle > 0) {
                sweepAngle -= 360.0f;
            }
        } else {
            if (sweepAngle < 0) {
                sweepAngle += 360.0f;
            }
        }
        
        // 创建包围矩形
        SkRect rect = SkRect::MakeXYWH(
            static_cast<SkScalar>(x - radius), static_cast<SkScalar>(y - radius),
            static_cast<SkScalar>(radius * 2), static_cast<SkScalar>(radius * 2)
        );
        
        // 添加圆弧到路径
        m_skiaPath->addArc(rect, startDegrees, sweepAngle);
        
        // 更新当前位置到弧的终点
        m_lastX = x + std::cos(endAngle) * radius;
        m_lastY = y + std::sin(endAngle) * radius;
    }

    // ========== 路径操作方法 ==========
    
    void Path2D::closePath() {
        m_skiaPath->close();
    }

    void Path2D::clear() {
        m_skiaPath->reset();
        m_lastX = 0.0;
        m_lastY = 0.0;
        m_hasFill = false;
        m_hasGradientFill = false;
    }

    // ========== 填充相关方法 ==========
    
    void Path2D::beginFill(uint32_t color, double alpha) {
        m_hasFill = true;
        m_hasGradientFill = false;
        m_fillColor = color;
        m_fillAlpha = alpha;
        updateFillPaint();
    }

    void Path2D::beginGradientFill(const std::string& type,
                                  const std::vector<uint32_t>& colors,
                                  const std::vector<double>& alphas,
                                  const std::vector<uint8_t>& ratios,
                                  const Matrix* matrix) {
        m_hasFill = true;
        m_hasGradientFill = true;
        m_gradientType = type;
        m_gradientColors = colors;
        m_gradientAlphas = alphas;
        m_gradientRatios = ratios;
        
        if (matrix) {
            m_gradientMatrix = std::make_unique<Matrix>(*matrix);
        } else {
            m_gradientMatrix.reset();
        }
        
        createGradientShader();
    }

    void Path2D::endFill() {
        // 填充结束，可以在这里做一些清理工作
    }

    // ========== 状态管理 ==========
    
    bool Path2D::isEmpty() const {
        return m_skiaPath->isEmpty();
    }

    void Path2D::setFillEvenOdd(bool enabled) {
        m_fillEvenOdd = enabled;
        if (m_skiaPath) {
            m_skiaPath->setFillType(enabled ? SkPathFillType::kEvenOdd : SkPathFillType::kWinding);
        }
    }

    bool Path2D::booleanOp(const Path2D& other, const std::string& op) {
        if (!m_skiaPath || !other.m_skiaPath) return false;
        SkPath result;
        SkPathOp skop;
        if (op == "union") skop = SkPathOp::kUnion_SkPathOp;
        else if (op == "intersect") skop = SkPathOp::kIntersect_SkPathOp;
        else if (op == "difference") skop = SkPathOp::kDifference_SkPathOp;
        else if (op == "xor") skop = SkPathOp::kXOR_SkPathOp;
        else if (op == "reverse_difference") skop = SkPathOp::kReverseDifference_SkPathOp;
        else return false;

        bool ok = Op(*m_skiaPath, *other.m_skiaPath, skop, &result);
        if (ok) {
            *m_skiaPath = result;
            // 维持当前填充规则设置
            m_skiaPath->setFillType(m_fillEvenOdd ? SkPathFillType::kEvenOdd : SkPathFillType::kWinding);
        }
        return ok;
    }

    // ========== 私有辅助方法 ==========
    
    void Path2D::updateFillPaint() {
        if (!m_hasFill) {
            return;
        }
        
        if (m_hasGradientFill) {
            createGradientShader();
        } else {
            // 设置纯色填充
            uint8_t r = (m_fillColor >> 16) & 0xFF;
            uint8_t g = (m_fillColor >> 8) & 0xFF;
            uint8_t b = m_fillColor & 0xFF;
            uint8_t a = static_cast<uint8_t>(m_fillAlpha * 255);
            
            SkColor skColor = SkColorSetARGB(a, r, g, b);
            m_fillPaint->setColor(skColor);
            m_fillPaint->setShader(nullptr);  // 清除之前的着色器
        }
    }

    void Path2D::createGradientShader() {
        if (!m_hasGradientFill || m_gradientColors.empty()) {
            return;
        }
        
        // 转换颜色和位置数组
        std::vector<SkColor> skColors;
        std::vector<SkScalar> positions;
        
        for (size_t i = 0; i < m_gradientColors.size(); i++) {
            uint32_t color = m_gradientColors[i];
            double alpha = (i < m_gradientAlphas.size()) ? m_gradientAlphas[i] : 1.0;
            uint8_t ratio = (i < m_gradientRatios.size()) ? m_gradientRatios[i] : static_cast<uint8_t>(i * 255 / (m_gradientColors.size() - 1));
            
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;
            uint8_t a = static_cast<uint8_t>(alpha * 255);
            
            skColors.push_back(SkColorSetARGB(a, r, g, b));
            positions.push_back(static_cast<SkScalar>(ratio) / 255.0f);
        }
        
        sk_sp<SkShader> shader;
        
        if (m_gradientType == "linear") {
            // 线性渐变
            SkPoint points[2] = {
                SkPoint::Make(0, 0),
                SkPoint::Make(100, 0)  // 默认水平渐变
            };
            
            shader = SkGradientShader::MakeLinear(
                points, skColors.data(), positions.data(), 
                static_cast<int>(skColors.size()), 
                SkTileMode::kClamp
            );
        } else if (m_gradientType == "radial") {
            // 径向渐变
            SkPoint center = SkPoint::Make(50, 50);  // 默认中心点
            SkScalar radius = 50;  // 默认半径
            
            shader = SkGradientShader::MakeRadial(
                center, radius,
                skColors.data(), positions.data(), 
                static_cast<int>(skColors.size()), 
                SkTileMode::kClamp
            );
        }
        
        // 应用变换矩阵
        if (m_gradientMatrix && shader) {
            SkMatrix skMatrix;
            skMatrix.setAll(
                static_cast<SkScalar>(m_gradientMatrix->a), static_cast<SkScalar>(m_gradientMatrix->c), static_cast<SkScalar>(m_gradientMatrix->tx),
                static_cast<SkScalar>(m_gradientMatrix->b), static_cast<SkScalar>(m_gradientMatrix->d), static_cast<SkScalar>(m_gradientMatrix->ty),
                0, 0, 1
            );
            shader = shader->makeWithLocalMatrix(skMatrix);
        }
        
        m_fillPaint->setShader(shader);
    }

} // namespace sys
} // namespace egret
