#pragma once
#include "player/SystemRenderer.hpp"
#include "player/NormalBitmapNode.hpp"
#include "player/SkiaRenderBuffer.hpp"
#include <memory>
#include <unordered_map>

// Skia前向声明/包含（用于sk_sp缓存）
class SkCanvas;
class SkPaint;
class SkPath;
#include <include/core/SkRefCnt.h>
#include <include/core/SkImage.h>

namespace egret {
namespace sys {

    // 前向声明
    class GraphicsNode;

    /**
     * SkiaRenderer - 基于Skia的高性能渲染器实现
     * 基于TypeScript egret.CanvasRenderer完整翻译到Skia API
     * 支持完整的2D渲染功能，包括位图、文本、矢量图形、滤镜等
     */
    class SkiaRenderer : public SystemRenderer {
    public:
        SkiaRenderer();
        virtual ~SkiaRenderer();
        
        // ========== SystemRenderer接口实现 ==========
        
        /**
         * 渲染一个显示对象到渲染缓冲区
         */
        int render(DisplayObject* displayObject, RenderBuffer* buffer, const Matrix& matrix, bool forRenderTexture = false) override;
        
        /**
         * 将RenderNode绘制到渲染缓冲区
         */
        void drawNodeToBuffer(RenderNode* node, RenderBuffer* buffer, const Matrix& matrix, bool forHitTest = false) override;
        
        /**
         * 清空渲染状态
         */
        void renderClear() override;

        /**
         * 失效BitmapData对应的SkImage缓存
         */
        void invalidateBitmapData(BitmapData* bmp) override;
        
        // ========== Skia特有方法 ==========
        
        /**
         * 获取当前Skia画布（调试用）
         */
        SkCanvas* getCurrentCanvas() const { return m_currentCanvas; }
        
        /**
         * 设置抗锯齿开关
         */
        void setAntiAlias(bool enabled) { m_antiAlias = enabled; }
        bool getAntiAlias() const { return m_antiAlias; }
        
        /**
         * 设置图像平滑开关
         */
        void setImageSmoothing(bool enabled) { m_imageSmoothing = enabled; }
        bool getImageSmoothing() const { return m_imageSmoothing; }
        
    private:
        // ========== 私有渲染方法 ==========
        
        /**
         * 绘制显示对象的核心逻辑
         * @param displayObject 显示对象
         * @param canvas Skia画布
         * @param offsetX X轴偏移
         * @param offsetY Y轴偏移
         * @param isStage 是否为舞台对象
         * @return 绘制调用次数
         */
        int drawDisplayObject(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY, bool isStage = false);
        
        /**
         * 渲染RenderNode到Skia画布
         * @param node 渲染节点
         * @param canvas Skia画布
         * @param forHitTest 是否用于碰撞检测
         * @return 绘制调用次数
         */
        int renderNode(RenderNode* node, SkCanvas* canvas, bool forHitTest = false);
        
        /**
         * 渲染位图节点
         * @param node 位图节点
         * @param canvas Skia画布
         * @return 绘制调用次数
         */
        int renderBitmap(BitmapNode* node, SkCanvas* canvas);
        int renderNormalBitmap(NormalBitmapNode* node, SkCanvas* canvas);
        
        /**
         * 渲染文本节点
         * @param node 文本节点
         * @param canvas Skia画布
         */
        void renderText(TextNode* node, SkCanvas* canvas);
        
        /**
         * 渲染矢量图形节点
         * @param node 图形节点
         * @param canvas Skia画布
         * @param forHitTest 是否用于碰撞检测
         * @return 绘制调用次数
         */
        int renderGraphics(GraphicsNode* node, SkCanvas* canvas, bool forHitTest = false);
        
        /**
         * 渲染组节点
         * @param node 组节点
         * @param canvas Skia画布
         * @return 绘制调用次数
         */
        int renderGroup(GroupNode* node, SkCanvas* canvas);
        
        /**
         * 渲染网格节点
         * @param node 网格节点
         * @param canvas Skia画布
         * @return 绘制调用次数
         */
        int renderMesh(MeshNode* node, SkCanvas* canvas);
        
        // ========== 滤镜和特效渲染 ==========
        
        /**
         * 使用滤镜绘制显示对象
         */
        int drawWithFilter(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY);
        
        /**
         * 使用裁剪绘制显示对象
         */
        int drawWithClip(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY);
        
        /**
         * 使用滚动矩形绘制显示对象
         */
        int drawWithScrollRect(DisplayObject* displayObject, SkCanvas* canvas, double offsetX, double offsetY);
        
        // ========== 辅助工具方法 ==========
        
        /**
         * 创建Skia渲染缓冲区（类型安全）
         */
        std::shared_ptr<SkiaRenderBuffer> createSkiaRenderBuffer(double width, double height, bool useForFilters = false);
        
        /**
         * 将egret.Matrix转换为Skia的SkMatrix
         */
        void convertToSkMatrix(const Matrix& egretMatrix, SkCanvas* canvas);
        
        /**
         * 设置Skia画笔属性
         */
        void setupPaint(SkPaint& paint, double alpha = 1.0, int blendMode = 0, bool antiAlias = true);
        
        /**
         * 颜色工具：将ARGB整数转换为Skia颜色
         */
        uint32_t convertColor(uint32_t color, double alpha = 1.0);
        
        // ========== 私有成员变量 ==========
        
        SkCanvas* m_currentCanvas = nullptr;           // 当前正在使用的画布
        int m_nestLevel = 0;                           // 渲染嵌套层次
        
        // 渲染设置
        bool m_antiAlias = true;                       // 抗锯齿开关
        bool m_imageSmoothing = true;                  // 图像平滑开关
        
        // 对象池 - 缓存临时渲染缓冲区
        std::vector<std::shared_ptr<SkiaRenderBuffer>> m_renderBufferPool;
        std::vector<std::shared_ptr<SkiaRenderBuffer>> m_filterBufferPool;
        
        // Skia资源
        std::unique_ptr<SkPaint> m_defaultPaint;       // 默认画笔
        std::unique_ptr<SkPath> m_tempPath;            // 临时路径对象

        // BitmapData -> SkImage 简易缓存（当前未做脏标记失效，后续可接入BitmapData::invalidate）
        std::unordered_map<const void*, sk_sp<SkImage>> m_imageCache;

        // 从BitmapData构建或获取缓存的SkImage
        sk_sp<SkImage> getOrCreateSkImage(class BitmapData* bmp);
        
        // 常量定义
        static constexpr int MAX_BUFFER_POOL_SIZE = 6; // 最大缓冲区池大小
        static constexpr uint32_t HIT_TEST_COLOR = 0xFF000000; // 碰撞检测颜色（黑色）
    };
    
    /**
     * 创建Skia渲染器实例
     */
    std::unique_ptr<SkiaRenderer> createSkiaRenderer();

} // namespace sys
} // namespace egret
