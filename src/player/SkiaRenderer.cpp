#include "player/SkiaRenderer.hpp"
#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "geom/Rectangle.hpp"
#include "sys/GraphicsNode.hpp"
#include "sys/Path2D.hpp"
#include "sys/StrokePath.hpp"
#include "player/nodes/TextNode.hpp"
#include "player/nodes/BitmapNode.hpp"
#include "player/nodes/GroupNode.hpp"
#include "player/nodes/MeshNode.hpp"
#include "utils/Logger.hpp"

// Skia头文件
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkPath.h>
#include <include/core/SkSurface.h>
#include <include/core/SkImage.h>
#include <include/core/SkMatrix.h>
#include <include/core/SkColor.h>
#include <include/core/SkBlendMode.h>
#include <include/effects/SkImageFilters.h>

#include <algorithm>
#include <cmath>

namespace egret {
namespace sys {

    // ========== SkiaRenderer实现 ==========
    
    SkiaRenderer::SkiaRenderer() {
        // 初始化默认画笔
        m_defaultPaint = std::make_unique<SkPaint>();
        m_defaultPaint->setAntiAlias(m_antiAlias);
        // 注意：setFilterQuality 在新版本Skia中已过时，改用 SkSamplingOptions
        
        // 初始化临时路径
        m_tempPath = std::make_unique<SkPath>();
        
        // 预留对象池空间
        m_renderBufferPool.reserve(MAX_BUFFER_POOL_SIZE);
        m_filterBufferPool.reserve(MAX_BUFFER_POOL_SIZE);
    }
    
    SkiaRenderer::~SkiaRenderer() {
        // RAII会自动清理unique_ptr
        m_renderBufferPool.clear();
        m_filterBufferPool.clear();
    }
    
    // ========== SystemRenderer接口实现 ==========
    
    int SkiaRenderer::render(DisplayObject* displayObject, RenderBuffer* buffer, const Matrix& matrix, bool forRenderTexture) {
        EGRET_DEBUG("Starting");
        
        if (!displayObject || !buffer) {
            EGRET_WARN("displayObject or buffer is null");
            return 0;
        }
        
        EGRET_DEBUGF("DisplayObject: x={}, y={}, visible={}, forRT={}", 
                    displayObject->getX(), displayObject->getY(), displayObject->getVisible(), forRenderTexture);
        
        m_nestLevel++;
        EGRET_DEBUGF("Nest level: {}", m_nestLevel);
        
        // 获取Skia画布
        SkCanvas* canvas = static_cast<SkCanvas*>(buffer->getSurface());
        if (!canvas) {
            EGRET_ERROR("Failed to get SkCanvas from buffer");
            m_nestLevel--;
            return 0;
        }
        
        EGRET_DEBUG("Got SkCanvas successfully");
        
        m_currentCanvas = canvas;
        DisplayObject* root = forRenderTexture ? displayObject : nullptr;
        
        // 保存画布状态并应用变换矩阵
        canvas->save();
        EGRET_DEBUG("Apply matrix");
        
        SkMatrix skMatrix;
        skMatrix.setAll(
            SkDoubleToScalar(matrix.getA()), SkDoubleToScalar(matrix.getC()), SkDoubleToScalar(matrix.getTx()),
            SkDoubleToScalar(matrix.getB()), SkDoubleToScalar(matrix.getD()), SkDoubleToScalar(matrix.getTy()),
            SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
        );
        canvas->concat(skMatrix);
        
        EGRET_DEBUG("Call drawDisplayObject");
        // 绘制显示对象
        int drawCalls = drawDisplayObject(displayObject, canvas, 0, 0, true);
        EGRET_DEBUGF("drawDisplayObject returned {} draw calls", drawCalls);
        
        // 恢复画布状态
        canvas->restore();
        EGRET_DEBUG("Restore canvas");
        
        m_nestLevel--;
        
        // 在最外层清理对象池
        if (m_nestLevel == 0) {
            EGRET_DEBUG("Cleanup pools (nest 0)");
            // 限制缓冲区池大小
            if (m_renderBufferPool.size() > MAX_BUFFER_POOL_SIZE) {
                m_renderBufferPool.resize(MAX_BUFFER_POOL_SIZE);
            }
            if (m_filterBufferPool.size() > MAX_BUFFER_POOL_SIZE) {
                m_filterBufferPool.resize(MAX_BUFFER_POOL_SIZE);
            }
            
            // 重置缓冲区大小（释放内存）
            for (auto& buffer : m_renderBufferPool) {
                if (buffer) {
                    buffer->resize(0, 0);
                }
            }
            for (auto& buffer : m_filterBufferPool) {
                if (buffer) {
                    buffer->resize(0, 0);
                }
            }
        }
        
        m_currentCanvas = nullptr;
        EGRET_DEBUGF("Finished with {} draw calls", drawCalls);
        return drawCalls;
    }
    
    void SkiaRenderer::drawNodeToBuffer(RenderNode* node, RenderBuffer* buffer, const Matrix& matrix, bool forHitTest) {
        if (!node || !buffer) {
            return;
        }
        
        SkCanvas* canvas = static_cast<SkCanvas*>(buffer->getSurface());
        if (!canvas) {
            return;
        }
        
        // 保存状态并设置变换
        canvas->save();
        
        SkMatrix skMatrix;
        skMatrix.setAll(
            SkDoubleToScalar(matrix.getA()), SkDoubleToScalar(matrix.getC()), SkDoubleToScalar(matrix.getTx()),
            SkDoubleToScalar(matrix.getB()), SkDoubleToScalar(matrix.getD()), SkDoubleToScalar(matrix.getTy()),
            SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
        );
        canvas->setMatrix(skMatrix);
        
        // 渲染节点
        renderNode(node, canvas, forHitTest);
        
        // 恢复状态
        canvas->restore();
    }
    
    void SkiaRenderer::renderClear() {
        // 清空当前状态
        m_currentCanvas = nullptr;
    }
    
    // ========== 私有渲染方法实现 ==========
    
    int SkiaRenderer::drawDisplayObject(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY, bool isStage) {
        EGRET_DEBUGF("Start: offsetX={}, offsetY={}, isStage={}", 
                    offsetX, offsetY, isStage);
        
        if (!displayObject || !canvas) {
            EGRET_WARN("displayObject or canvas is null");
            return 0;
        }
        
        EGRET_DEBUGF("DisplayObject: x={}, y={}, visible={}", 
                    displayObject->getX(), displayObject->getY(), displayObject->getVisible());
        
        int drawCalls = 0;
        RenderNode* node = nullptr;
        
        // 获取显示列表或渲染节点
        auto displayList = displayObject->getDisplayList();
        if (displayList && !isStage) {
            EGRET_DEBUG("Has DisplayList (container)");
            // 容器对象：检查是否需要重绘到自己的缓存
            if (displayObject->isCacheDirty() || displayObject->isRenderDirty()) {
                EGRET_DEBUG("Container dirty, redraw DisplayList");
                drawCalls += displayList->drawToSurface();
            }
            node = displayList->getRenderNode().get();
        } else {
            EGRET_DEBUG("Get direct RenderNode");
            // 普通显示对象：直接获取RenderNode
            node = displayObject->getRenderNode().get();
        }
        
        if (node) {
            EGRET_DEBUGF("RenderNode type: {}", 
                        static_cast<int>(node->getType()));
        } else {
            EGRET_DEBUG("No RenderNode");
        }
        
        // 清除脏标记
        displayObject->setCacheDirty(false);
        
        // 渲染当前对象的RenderNode
        if (node) {
            EGRET_DEBUG("RenderNode begin");
            canvas->save();
            canvas->translate(SkDoubleToScalar(offsetX), SkDoubleToScalar(offsetY));
            
            int nodeDrawCalls = renderNode(node, canvas, false);
            EGRET_DEBUGF("RenderNode drawCalls={}", nodeDrawCalls);
            drawCalls += nodeDrawCalls;
            
            canvas->restore();
        }
        
        // 关键：递归渲染所有子对象
        auto container = dynamic_cast<DisplayObjectContainer*>(displayObject);
        if (container) {
            int numChildren = container->getNumChildren();
            EGRET_DEBUGF("Children: {}", numChildren);
            
            for (int i = 0; i < numChildren; i++) {
                auto child = container->getChildAt(i);
                if (!child) {
                    EGRET_WARNF("Child {} is null", i);
                    continue;
                }
                
                if (!child->getVisible()) {
                    EGRET_DEBUGF("Child {} not visible, skip", i);
                    continue;
                }
                
                EGRET_DEBUGF("Render child {}: x={}, y={}", 
                           i, child->getX(), child->getY());
                
                // 计算子对象变换
                canvas->save();
                
                double childOffsetX, childOffsetY;
                if (child->shouldUseTransform()) {
                    EGRET_DEBUGF("Child {} uses transform matrix", i);
                    // 使用完整变换矩阵
                    Matrix matrix = child->getMatrix();
                    SkMatrix childMatrix;
                    childMatrix.setAll(
                        SkDoubleToScalar(matrix.getA()), SkDoubleToScalar(matrix.getC()), SkDoubleToScalar(matrix.getTx()),
                        SkDoubleToScalar(matrix.getB()), SkDoubleToScalar(matrix.getD()), SkDoubleToScalar(matrix.getTy()),
                        SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
                    );
                    canvas->concat(childMatrix);
                    
                    childOffsetX = -child->getAnchorOffsetX();
                    childOffsetY = -child->getAnchorOffsetY();
                } else {
                    EGRET_DEBUGF("Child {} uses simple translation", i);
                    // 简单偏移
                    childOffsetX = offsetX + child->getX() - child->getAnchorOffsetX();
                    childOffsetY = offsetY + child->getY() - child->getAnchorOffsetY();
                }
                
                EGRET_DEBUGF("Child {} offsets: x={}, y={}", 
                           i, childOffsetX, childOffsetY);
                
                // 处理透明度
                if (child->getAlpha() < 1.0) {
                    EGRET_DEBUGF("Child {} alpha={}", 
                               i, child->getAlpha());
                    canvas->saveLayerAlpha(nullptr, static_cast<U8CPU>(child->getAlpha() * 255));
                }
                
                // 递归调用
                int childDrawCalls = drawDisplayObject(child, canvas, childOffsetX, childOffsetY, false);
                EGRET_DEBUGF("Child {} drawCalls={}", 
                           i, childDrawCalls);
                drawCalls += childDrawCalls;
                
                canvas->restore();
            }
        } else {
            EGRET_DEBUG("Not a container");
        }
        
        EGRET_DEBUGF("Total drawCalls={}", drawCalls);
        return drawCalls;
    }
    
    int SkiaRenderer::renderNode(RenderNode* node, SkCanvas* canvas, bool forHitTest) {
        if (!node || !canvas) {
            return 0;
        }
        
        int drawCalls = 0;
        
        switch (node->getType()) {
            case RenderNodeType::BitmapNode:
                drawCalls = renderBitmap(static_cast<BitmapNode*>(node), canvas);
                break;
            case RenderNodeType::TextNode:
                drawCalls = 1;
                renderText(static_cast<TextNode*>(node), canvas);
                break;
            case RenderNodeType::GraphicsNode:
                drawCalls = renderGraphics(static_cast<GraphicsNode*>(node), canvas, forHitTest);
                break;
            case RenderNodeType::GroupNode:
                drawCalls = renderGroup(static_cast<GroupNode*>(node), canvas);
                break;
            case RenderNodeType::MeshNode:
                drawCalls = renderMesh(static_cast<MeshNode*>(node), canvas);
                break;
            case RenderNodeType::NormalBitmapNode:
                drawCalls = renderBitmap(static_cast<BitmapNode*>(node), canvas);
                break;
        }
        
        return drawCalls;
    }
    
    int SkiaRenderer::renderBitmap(BitmapNode* node, SkCanvas* canvas) {
        if (!node || !canvas) {
            return 0;
        }
        
        // 获取图像数据
        void* imageData = node->getImage();
        if (!imageData) {
            return 0;
        }
        
        // TODO: 将imageData转换为SkImage
        // SkImage* skImage = static_cast<SkImage*>(imageData);
        // if (!skImage) {
        //     return 0;
        // }
        
        SkPaint paint;
        setupPaint(paint);
        
        // 获取绘制数据
        const auto& drawData = node->getDrawData();
        if (drawData.size() < 8) {
            return 0;
        }
        
        // 解析绘制参数：sourceX, sourceY, sourceW, sourceH, drawX, drawY, drawW, drawH
        SkRect srcRect = SkRect::MakeXYWH(
            SkDoubleToScalar(drawData[0]), SkDoubleToScalar(drawData[1]),
            SkDoubleToScalar(drawData[2]), SkDoubleToScalar(drawData[3])
        );
        
        SkRect dstRect = SkRect::MakeXYWH(
            SkDoubleToScalar(drawData[4]), SkDoubleToScalar(drawData[5]),
            SkDoubleToScalar(drawData[6]), SkDoubleToScalar(drawData[7])
        );
        
        // TODO: 使用Skia绘制图像
        // canvas->drawImageRect(skImage, srcRect, dstRect, &paint);
        
        return 1;
    }
    
    void SkiaRenderer::renderText(TextNode* node, SkCanvas* canvas) {
        if (!node || !canvas) {
            return;
        }
        
        // TODO: 实现文本渲染
        // 需要集成Skia的文本渲染API：SkTextBlob, SkFont等
        // const auto& drawData = node->getDrawData();
        // 解析文本内容、位置、格式等信息
        // 设置字体、颜色、描边等属性
        // 使用canvas->drawTextBlob()进行绘制
    }
    
    int SkiaRenderer::renderGraphics(GraphicsNode* node, SkCanvas* canvas, bool forHitTest) {
        if (!node || !canvas) {
            return 0;
        }
        
        // 获取绘图数据（Path2D对象列表）
        const auto& drawData = node->getDrawData();
        if (drawData.empty()) {
            return 0;
        }
        
        int drawCalls = 0;
        
        // 遍历所有Path2D对象
        for (const auto& path : drawData) {
            if (!path || path->isEmpty()) {
                continue;
            }
            
            // 获取Skia路径对象
            SkPath* skiaPath = path->getSkiaPath();
            if (!skiaPath) {
                continue;
            }
            
            // 绘制填充
            if (path->hasFill()) {
                SkPaint* fillPaint = path->getFillPaint();
                if (fillPaint) {
                    fillPaint->setAntiAlias(true);
                    canvas->drawPath(*skiaPath, *fillPaint);
                    drawCalls++;
                }
            }
            
            // 检查是否是StrokePath并处理描边
            auto strokePath = std::dynamic_pointer_cast<sys::StrokePath>(path);
            if (strokePath && strokePath->hasStroke()) {
                SkPaint* strokePaint = strokePath->getStrokePaint();
                if (strokePaint && strokePath->getThickness() > 0) {
                    strokePaint->setAntiAlias(true);
                    canvas->drawPath(*skiaPath, *strokePaint);
                    drawCalls++;
                }
            }
        }
        
        return drawCalls;
    }
    
    int SkiaRenderer::renderGroup(GroupNode* node, SkCanvas* canvas) {
        if (!node || !canvas) {
            return 0;
        }
        
        // TODO: 实现组节点渲染
        // 应用组变换矩阵
        // 递归渲染所有子节点
        
        return 1;
    }
    
    int SkiaRenderer::renderMesh(MeshNode* node, SkCanvas* canvas) {
        if (!node || !canvas) {
            return 0;
        }
        
        // TODO: 实现网格渲染
        // 使用Skia的顶点渲染功能
        // canvas->drawVertices()
        
        return 1;
    }
    
    // ========== 滤镜和特效渲染实现 ==========
    
    int SkiaRenderer::drawWithFilter(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY) {
        // TODO: 实现滤镜渲染
        // 创建临时画布
        // 应用滤镜效果
        // 绘制结果到目标画布
        return 0;
    }
    
    int SkiaRenderer::drawWithClip(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY) {
        // TODO: 实现裁剪渲染
        // 使用canvas->clipRect()或canvas->clipPath()
        return 0;
    }
    
    int SkiaRenderer::drawWithScrollRect(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY) {
        // TODO: 实现滚动矩形渲染
        // 设置裁剪区域并调整偏移
        return 0;
    }
    
    // ========== 辅助工具方法实现 ==========
    
    std::shared_ptr<SkiaRenderBuffer> SkiaRenderer::createSkiaRenderBuffer(double width, double height, bool useForFilters) {
        auto& pool = useForFilters ? m_filterBufferPool : m_renderBufferPool;
        
        // 尝试从对象池获取
        if (!pool.empty()) {
            auto buffer = pool.back();
            pool.pop_back();
            buffer->resize(width, height, true);
            return buffer;
        }
        
        // 创建新的Skia渲染缓冲区
        return ::egret::sys::createSkiaRenderBuffer(width, height);
    }
    
    void SkiaRenderer::convertToSkMatrix(const Matrix& egretMatrix, SkCanvas* canvas) {
        SkMatrix skMatrix;
        skMatrix.setAll(
            SkDoubleToScalar(egretMatrix.getA()), SkDoubleToScalar(egretMatrix.getC()), SkDoubleToScalar(egretMatrix.getTx()),
            SkDoubleToScalar(egretMatrix.getB()), SkDoubleToScalar(egretMatrix.getD()), SkDoubleToScalar(egretMatrix.getTy()),
            SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
        );
        canvas->setMatrix(skMatrix);
    }
    
    void SkiaRenderer::setupPaint(SkPaint& paint, double alpha, int blendMode, bool antiAlias) {
        paint.setAntiAlias(antiAlias && m_antiAlias);
        // 注意：setFilterQuality 在新版本Skia中已过时，现在用 SkSamplingOptions 设置采样
        paint.setAlpha(static_cast<U8CPU>(alpha * 255));
        
        // 设置混合模式
        switch (blendMode) {
            case 0: // NORMAL
                paint.setBlendMode(SkBlendMode::kSrcOver);
                break;
            case 1: // ADD
                paint.setBlendMode(SkBlendMode::kPlus);
                break;
            case 2: // ERASE
                paint.setBlendMode(SkBlendMode::kDstOut);
                break;
            default:
                paint.setBlendMode(SkBlendMode::kSrcOver);
                break;
        }
    }
    
    uint32_t SkiaRenderer::convertColor(uint32_t color, double alpha) {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        uint8_t a = static_cast<uint8_t>(alpha * 255);
        
        return SkColorSetARGB(a, r, g, b);
    }
    
    // ========== 工厂方法实现 ==========
    
    std::unique_ptr<SkiaRenderer> createSkiaRenderer() {
        return std::make_unique<SkiaRenderer>();
    }

} // namespace sys
} // namespace egret
