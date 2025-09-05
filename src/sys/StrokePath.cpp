#include "sys/StrokePath.hpp"

// Skia includes
#include <include/core/SkPaint.h>
#include <include/core/SkPathEffect.h>
#include <include/effects/SkDashPathEffect.h>

namespace egret {
namespace sys {

    StrokePath::StrokePath() : Path2D() {
        // 创建线条画笔
        m_strokePaint = std::make_unique<SkPaint>();
        
        // 初始化画笔
        m_strokePaint->setAntiAlias(true);
        m_strokePaint->setStyle(SkPaint::kStroke_Style);
    }

    StrokePath::~StrokePath() {
        // 智能指针会自动释放资源
    }

    // ========== 线条样式方法 ==========
    
    void StrokePath::setLineStyle(double thickness, 
                                 uint32_t color, 
                                 double alpha,
                                 const std::string& caps,
                                 const std::string& joints,
                                 double miterLimit,
                                 const std::vector<double>& lineDash) {
        m_hasStroke = true;
        m_thickness = thickness;
        m_strokeColor = color;
        m_strokeAlpha = alpha;
        m_caps = caps;
        m_joints = joints;
        m_miterLimit = miterLimit;
        m_lineDash = lineDash;
        
        updateStrokePaint();
    }

    // ========== 私有辅助方法 ==========
    
    void StrokePath::updateStrokePaint() {
        if (!m_hasStroke) {
            return;
        }
        
        // 设置线条粗细
        m_strokePaint->setStrokeWidth(static_cast<SkScalar>(m_thickness));
        
        // 设置线条颜色
        uint8_t r = (m_strokeColor >> 16) & 0xFF;
        uint8_t g = (m_strokeColor >> 8) & 0xFF;
        uint8_t b = m_strokeColor & 0xFF;
        uint8_t a = static_cast<uint8_t>(m_strokeAlpha * 255);
        
        SkColor skColor = SkColorSetARGB(a, r, g, b);
        m_strokePaint->setColor(skColor);
        
        // 设置端点样式
        setSkiaStrokeCap(m_caps);
        
        // 设置连接样式
        setSkiaStrokeJoin(m_joints);
        
        // 设置斜接限制
        m_strokePaint->setStrokeMiter(static_cast<SkScalar>(m_miterLimit));
        
        // 设置虚线效果
        if (!m_lineDash.empty()) {
            setSkiaPathEffect(m_lineDash);
        }
    }

    void StrokePath::setSkiaStrokeCap(const std::string& caps) {
        if (caps == "round") {
            m_strokePaint->setStrokeCap(SkPaint::kRound_Cap);
        } else if (caps == "square") {
            m_strokePaint->setStrokeCap(SkPaint::kSquare_Cap);
        } else { // "none" or default
            m_strokePaint->setStrokeCap(SkPaint::kButt_Cap);
        }
    }

    void StrokePath::setSkiaStrokeJoin(const std::string& joints) {
        if (joints == "round") {
            m_strokePaint->setStrokeJoin(SkPaint::kRound_Join);
        } else if (joints == "bevel") {
            m_strokePaint->setStrokeJoin(SkPaint::kBevel_Join);
        } else { // "miter" or default
            m_strokePaint->setStrokeJoin(SkPaint::kMiter_Join);
        }
    }

    void StrokePath::setSkiaPathEffect(const std::vector<double>& lineDash) {
        if (lineDash.empty()) {
            m_strokePaint->setPathEffect(nullptr);
            return;
        }
        
        // 转换虚线数组为Skia格式
        std::vector<SkScalar> intervals;
        for (double dash : lineDash) {
            intervals.push_back(static_cast<SkScalar>(dash));
        }
        
        // 创建虚线效果
        auto pathEffect = SkDashPathEffect::Make(
            intervals.data(), 
            static_cast<int>(intervals.size()), 
            0  // phase offset
        );
        
        m_strokePaint->setPathEffect(pathEffect);
    }

} // namespace sys
} // namespace egret