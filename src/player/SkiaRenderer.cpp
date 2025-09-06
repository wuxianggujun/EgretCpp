#include "player/SkiaRenderer.hpp"
#include "display/DisplayObject.hpp"
#include "display/DisplayObjectContainer.hpp"
#include "geom/Rectangle.hpp"
#include "display/Bitmap.hpp"
#include "sys/GraphicsNode.hpp"
#include "sys/Path2D.hpp"
#include "sys/StrokePath.hpp"
#include "player/nodes/TextNode.hpp"
#include "player/nodes/BitmapNode.hpp"
#include "player/nodes/GroupNode.hpp"
#include "player/nodes/MeshNode.hpp"
#include "utils/Logger.hpp"
#include "display/BitmapData.hpp"

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
#include <include/core/SkData.h>
#include <include/core/SkSamplingOptions.h>
#include <include/core/SkImageInfo.h>

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

    void SkiaRenderer::invalidateBitmapData(BitmapData* bmp) {
        if (!bmp) return;
        const void* key = static_cast<const void*>(bmp);
        auto it = m_imageCache.find(key);
        if (it != m_imageCache.end()) {
            m_imageCache.erase(it);
        }
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

        // 在渲染当前对象之前，如果是Bitmap，准备其渲染节点数据
        if (auto bmpSelf = dynamic_cast<Bitmap*>(displayObject)) {
            bmpSelf->prepareRenderNode();
        }
        
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

        // 处理 Mask/ScrollRect 包裹渲染（与Egret语义对齐）
        Rectangle* scrollRect = displayObject->getScrollRect();
        DisplayObject* maskObj = displayObject->getMask();
        bool hasScroll = (scrollRect != nullptr);
        bool hasMask = (maskObj != nullptr);

        if ((hasScroll || hasMask) && !isStage) {
            // 原则：在对象本地坐标系中，按顺序处理 scrollRect（裁剪+平移）、mask（saveLayer + DstIn），再绘制内容
            canvas->save();
            canvas->translate(SkDoubleToScalar(offsetX), SkDoubleToScalar(offsetY));

            // 本地递归渲染(忽略mask/scrollRect)的lambda
            std::function<int(DisplayObject*)> renderRaw = [&](DisplayObject* obj) -> int {
                int calls = 0;
                if (!obj) return 0;
                auto n = obj->getRenderNode().get();
                if (n) {
                    calls += renderNode(n, canvas, false);
                }
                if (auto ctn = dynamic_cast<DisplayObjectContainer*>(obj)) {
                    int nchild = ctn->getNumChildren();
                    for (int i = 0; i < nchild; ++i) {
                        auto ch = ctn->getChildAt(i);
                        if (!ch || !ch->getVisible()) continue;
                        canvas->save();
                        if (ch->shouldUseTransform()) {
                            Matrix m2 = ch->getMatrix();
                            SkMatrix sm;
                            sm.setAll(
                                SkDoubleToScalar(m2.getA()), SkDoubleToScalar(m2.getC()), SkDoubleToScalar(m2.getTx()),
                                SkDoubleToScalar(m2.getB()), SkDoubleToScalar(m2.getD()), SkDoubleToScalar(m2.getTy()),
                                SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
                            );
                            canvas->concat(sm);
                        } else {
                            canvas->translate(SkDoubleToScalar(ch->getX()), SkDoubleToScalar(ch->getY()));
                        }
                        canvas->translate(SkDoubleToScalar(-ch->getAnchorOffsetX()), SkDoubleToScalar(-ch->getAnchorOffsetY()));
                        if (ch->getAlpha() < 1.0) {
                            canvas->saveLayerAlpha(nullptr, static_cast<U8CPU>(ch->getAlpha() * 255));
                        }
                        calls += renderRaw(ch);
                        canvas->restore();
                    }
                }
                return calls;
            };

            // ScrollRect：裁剪并平移
            if (hasScroll) {
                SkRect r = SkRect::MakeXYWH(
                    SkDoubleToScalar(scrollRect->getX()),
                    SkDoubleToScalar(scrollRect->getY()),
                    SkDoubleToScalar(scrollRect->getWidth()),
                    SkDoubleToScalar(scrollRect->getHeight())
                );
                canvas->clipRect(r, SkClipOp::kIntersect, true);
                canvas->translate(SkDoubleToScalar(-scrollRect->getX()), SkDoubleToScalar(-scrollRect->getY()));
            }

            if (hasMask) {
                // Layer L1：绘制内容
                canvas->saveLayer(nullptr, nullptr);
                drawCalls += renderRaw(displayObject);

                // Layer L2 (DstIn)：绘制遮罩
                SkPaint pm; pm.setBlendMode(SkBlendMode::kDstIn);
                canvas->saveLayer(nullptr, &pm);

                // 计算mask→object 的相对矩阵
                Matrix* maskCM = maskObj->getConcatenatedMatrix();
                Matrix* objCM = displayObject->getConcatenatedMatrix();
                Matrix rel = objCM->invert();
                rel.appendMatrix(*maskCM);
                SkMatrix m;
                m.setAll(
                    SkDoubleToScalar(rel.getA()), SkDoubleToScalar(rel.getC()), SkDoubleToScalar(rel.getTx()),
                    SkDoubleToScalar(rel.getB()), SkDoubleToScalar(rel.getD()), SkDoubleToScalar(rel.getTy()),
                    SkDoubleToScalar(0.0), SkDoubleToScalar(0.0), SkDoubleToScalar(1.0)
                );
                canvas->save();
                canvas->concat(m);
                // 绘制遮罩对象（任意DisplayObject），其alpha将作为蒙版
                drawCalls += renderRaw(maskObj);
                canvas->restore();

                canvas->restore(); // 结束L2，应用DstIn
                canvas->restore(); // 结束L1，提交内容
            } else {
                // 无遮罩：仅按scrollRect裁剪后渲染内容
                drawCalls += renderRaw(displayObject);
                canvas->restore();
            }
            return drawCalls;
        }

        // 普通路径：渲染当前对象的RenderNode
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
                drawCalls = renderNormalBitmap(static_cast<NormalBitmapNode*>(node), canvas);
                break;
        }
        
        return drawCalls;
    }
    
    int SkiaRenderer::renderBitmap(BitmapNode* node, SkCanvas* canvas) {
        if (!node || !canvas) {
            return 0;
        }
        
        // 从 BitmapNode 的 image 字段获取 BitmapData
        BitmapData* bmpDataPtr = reinterpret_cast<BitmapData*>(node->getImage());
        if (!bmpDataPtr) {
            return 0;
        }
        
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

        // 从缓存或像素构造SkImage
        sk_sp<SkImage> image = getOrCreateSkImage(bmpDataPtr);
        if (!image) {
            return 0;
        }

        // 平滑采样设置（BitmapNode 公有字段 smoothing）
        SkSamplingOptions sampling(node->smoothing ? SkFilterMode::kLinear : SkFilterMode::kNearest);

        // 绘制子区域到目标区域
        canvas->drawImageRect(image, srcRect, dstRect, sampling, &paint, SkCanvas::kStrict_SrcRectConstraint);

        return 1;
    }

    int SkiaRenderer::renderNormalBitmap(NormalBitmapNode* node, SkCanvas* canvas) {
        if (!node || !canvas) {
            return 0;
        }

        // 尝试从节点获取BitmapData
        BitmapData* bmpDataPtr = reinterpret_cast<BitmapData*>(node->getBitmapData());
        if (!bmpDataPtr) {
            // 兼容路径：有些节点可能把BitmapData放在image字段
            bmpDataPtr = reinterpret_cast<BitmapData*>(node->getImage());
        }
        if (!bmpDataPtr) {
            return 0;
        }

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

        // 从缓存或像素构造SkImage
        sk_sp<SkImage> image = getOrCreateSkImage(bmpDataPtr);
        if (!image) {
            return 0;
        }

        // 平滑采样设置（NormalBitmapNode 提供 isSmooth()）
        SkSamplingOptions sampling(node->isSmooth() ? SkFilterMode::kLinear : SkFilterMode::kNearest);

        // 绘制子区域到目标区域
        canvas->drawImageRect(image, srcRect, dstRect, sampling, &paint, SkCanvas::kStrict_SrcRectConstraint);

        return 1;
    }

    // ========== 辅助：获取或构建SkImage缓存 ==========
    sk_sp<SkImage> SkiaRenderer::getOrCreateSkImage(BitmapData* bmp) {
        if (!bmp) return nullptr;
        const void* key = static_cast<const void*>(bmp);
        auto it = m_imageCache.find(key);
        if (it != m_imageCache.end() && it->second) {
            return it->second;
        }

        int texW = bmp->getWidth();
        int texH = bmp->getHeight();
        if (texW <= 0 || texH <= 0) {
            return nullptr;
        }

        auto argb = bmp->getPixels(0, 0, texW, texH);
        if (static_cast<int>(argb.size()) != texW * texH) {
            return nullptr;
        }

        std::vector<uint8_t> rgba(static_cast<size_t>(texW) * static_cast<size_t>(texH) * 4);
        for (int i = 0; i < texW * texH; ++i) {
            uint32_t p = argb[static_cast<size_t>(i)];
            uint8_t a = (p >> 24) & 0xFF;
            uint8_t r = (p >> 16) & 0xFF;
            uint8_t g = (p >> 8) & 0xFF;
            uint8_t b = p & 0xFF;
            // 预乘Alpha
            rgba[static_cast<size_t>(i) * 4 + 0] = static_cast<uint8_t>((r * a + 127) / 255);
            rgba[static_cast<size_t>(i) * 4 + 1] = static_cast<uint8_t>((g * a + 127) / 255);
            rgba[static_cast<size_t>(i) * 4 + 2] = static_cast<uint8_t>((b * a + 127) / 255);
            rgba[static_cast<size_t>(i) * 4 + 3] = a;
        }

        SkImageInfo info = SkImageInfo::Make(texW, texH, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        size_t rowBytes = static_cast<size_t>(texW) * 4;
        sk_sp<SkData> data = SkData::MakeWithCopy(rgba.data(), rowBytes * static_cast<size_t>(texH));
        sk_sp<SkImage> image = SkImages::RasterFromData(info, data, rowBytes);
        if (image) {
            m_imageCache[key] = image;
        }
        return image;
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
