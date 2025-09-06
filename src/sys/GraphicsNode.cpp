#include "sys/GraphicsNode.hpp"
#include "sys/Path2D.hpp"
#include "sys/StrokePath.hpp"
#include "geom/Matrix.hpp"
#include <algorithm>

namespace egret {
namespace sys {

    GraphicsNode::GraphicsNode() : RenderNode(RenderNodeType::GraphicsNode) {
        // 初始化边界
        x = 0.0;
        y = 0.0;
        width = 0.0;
        height = 0.0;
        dirtyRender = false;
    }

    GraphicsNode::~GraphicsNode() {
        clean();
    }

    std::shared_ptr<Path2D> GraphicsNode::beginFill(uint32_t color, double alpha, 
                                                   std::shared_ptr<Path2D> beforePath) {
        // 创建填充路径
        auto fillPath = std::make_shared<Path2D>();
        // 设置填充属性，确保渲染器识别 hasFill 和 paint
        fillPath->beginFill(color, alpha);
        
        if (beforePath) {
            // 在指定路径前插入
            auto it = std::find(m_drawData.begin(), m_drawData.end(), beforePath);
            if (it != m_drawData.end()) {
                m_drawData.insert(it, fillPath);
            }
        } else {
            m_drawData.push_back(fillPath);
        }
        
        dirtyRender = true;
        return fillPath;
    }

    std::shared_ptr<Path2D> GraphicsNode::beginGradientFill(const std::string& type,
                                                           const std::vector<uint32_t>& colors,
                                                           const std::vector<double>& alphas,
                                                           const std::vector<double>& ratios,
                                                           const Matrix* matrix,
                                                           std::shared_ptr<Path2D> beforePath) {
        // 创建渐变填充路径
        auto gradientPath = std::make_shared<Path2D>();
        // 转换ratio到字节（0-255）
        std::vector<uint8_t> ratioBytes;
        ratioBytes.reserve(ratios.size());
        for (double r : ratios) {
            double clamped = std::max(0.0, std::min(255.0, r));
            ratioBytes.push_back(static_cast<uint8_t>(clamped));
        }
        // 设置渐变属性
        gradientPath->beginGradientFill(type, colors, alphas, ratioBytes, matrix);
        
        if (beforePath) {
            auto it = std::find(m_drawData.begin(), m_drawData.end(), beforePath);
            if (it != m_drawData.end()) {
                m_drawData.insert(it, gradientPath);
            }
        } else {
            m_drawData.push_back(gradientPath);
        }
        
        dirtyRender = true;
        return gradientPath;
    }

    std::shared_ptr<StrokePath> GraphicsNode::lineStyle(double thickness, uint32_t color, 
                                                       double alpha, const std::string& caps,
                                                       const std::string& joints, double miterLimit,
                                                       const std::vector<double>& lineDash) {
        auto strokePath = std::make_shared<StrokePath>();
        
        // 设置线条属性（确保 hasStroke 正确置位）
        strokePath->setLineStyle(thickness, color, alpha, caps, joints, miterLimit, lineDash);
        
        m_drawData.push_back(strokePath);
        dirtyRender = true;
        return strokePath;
    }

    void GraphicsNode::clear() {
        m_drawData.clear();
        
        // 重置边界
        x = 0.0;
        y = 0.0;
        width = 0.0;
        height = 0.0;
        
        dirtyRender = true;
    }

    void GraphicsNode::cleanBeforeRender() {
        // 对应TypeScript版本，不自动清空缓存
        // GraphicsNode需要手动调用clear()方法清空
    }

    void GraphicsNode::clean() {
        // 清除非绘制的缓存数据
        // TODO: 清理WebGL纹理等资源
        dirtyRender = true;
    }

    void GraphicsNode::render(void* renderer) {
        if (!renderer) {
            return;
        }
        
        // TODO: 实现实际的渲染逻辑
        // 遍历m_drawData中的所有路径进行渲染
        
        // 更新渲染计数
        m_renderCount++;
    }

} // namespace sys
} // namespace egret
