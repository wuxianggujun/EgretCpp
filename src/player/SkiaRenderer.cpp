#include "player/SkiaRenderer.hpp"
#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "geom/Rectangle.hpp"
#include "sys/GraphicsNode.hpp"
#include "player/nodes/TextNode.hpp"
#include "player/nodes/BitmapNode.hpp"
#include "player/nodes/GroupNode.hpp"
#include "player/nodes/MeshNode.hpp"

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
        if (!displayObject || !buffer) {
            return 0;
        }
        
        m_nestLevel++;
        
        // 获取Skia画布
        SkCanvas* canvas = static_cast<SkCanvas*>(buffer->getSurface());
        if (!canvas) {
            m_nestLevel--;
            return 0;
        }
        
        m_currentCanvas = canvas;
        DisplayObject* root = forRenderTexture ? displayObject : nullptr;
        
        // 保存画布状态并应用变换矩阵
        canvas->save();
        
        SkMatrix skMatrix;
        skMatrix.setAll(
            SkDoubleToScalar(matrix.getA()), SkDoubleToScalar(matrix.getC()), SkDoubleToScalar(matrix.getTx()),
            SkDoubleToScalar(matrix.getB()), SkDoubleToScalar(matrix.getD()), SkDoubleToScalar(matrix.getTy()),
            SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
        );
        canvas->concat(skMatrix);
        
        // 绘制显示对象
        int drawCalls = drawDisplayObject(displayObject, canvas, 0, 0, true);
        
        // 恢复画布状态
        canvas->restore();
        
        m_nestLevel--;
        
        // 在最外层清理对象池
        if (m_nestLevel == 0) {
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
        if (!displayObject || !canvas) {
            return 0;
        }
        
        int drawCalls = 0;
        RenderNode* node = nullptr;
        
        // 获取显示列表或渲染节点
        auto displayList = displayObject->getDisplayList();
        if (displayList && !isStage) {
            // TODO: 检查缓存脏标记
            // if (displayObject->getCacheDirty() || displayObject->getRenderDirty() || ...) {
                drawCalls += displayList->drawToSurface();
            // }
            node = displayList->getRenderNode().get();
        } else {
            // TODO: 获取显示对象的渲染节点
            // if (displayObject->getRenderDirty()) {
            //     node = displayObject->getRenderNode();
            // } else {
            //     node = displayObject->getCachedRenderNode();
            // }
        }
        
        // 渲染节点内容
        if (node) {
            drawCalls++;
            canvas->save();
            canvas->translate(SkDoubleToScalar(offsetX), SkDoubleToScalar(offsetY));
            
            renderNode(node, canvas);
            
            canvas->restore();
        }
        
        // 如果有显示列表且不是舞台，直接返回
        if (displayList && !isStage) {
            return drawCalls;
        }
        
        // 渲染子对象
        auto container = dynamic_cast<DisplayObjectContainer*>(displayObject);
        if (container) {
            // TODO: 获取子对象列表并渲染
            // auto children = container->getChildren();
            // for (auto child : children) {
            //     // 计算子对象变换
            //     double childOffsetX = offsetX + child->getX() - child->getAnchorOffsetX();
            //     double childOffsetY = offsetY + child->getY() - child->getAnchorOffsetY();
            //     
            //     // 处理alpha、混合模式、变换等
            //     canvas->save();
            //     
            //     if (child->getAlpha() != 1.0) {
            //         canvas->saveLayerAlpha(nullptr, static_cast<U8CPU>(child->getAlpha() * 255));
            //     }
            //     
            //     // 递归渲染子对象
            //     drawCalls += drawDisplayObject(child, canvas, childOffsetX, childOffsetY);
            //     
            //     canvas->restore();
            // }
        }
        
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
        
        // TODO: 实现矢量图形渲染
        // const auto& drawData = node->getDrawData();
        // for (auto& pathData : drawData) {
        //     switch (pathData.type) {
        //         case PathType::Fill:
        //             // 设置填充样式并绘制
        //             break;
        //         case PathType::Stroke:
        //             // 设置描边样式并绘制
        //             break;
        //         case PathType::GradientFill:
        //             // 设置渐变填充并绘制
        //             break;
        //     }
        // }
        
        return 1;
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