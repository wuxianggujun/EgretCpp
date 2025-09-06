#pragma once
#include "core/HashObject.hpp"
#include "geom/Matrix.hpp"
#include "player/RenderBuffer.hpp"
#include "player/RenderNode.hpp"
#include <memory>

namespace egret {
    // 前向声明
    class DisplayObject;
    class BitmapData;
    
namespace sys {

    /**
     * SystemRenderer渲染器接口
     * 基于TypeScript egret.sys.SystemRenderer接口实现
     * 统一的渲染器接口，支持不同的渲染后端（Skia、Canvas等）
     */
    class SystemRenderer : public HashObject {
    public:
        SystemRenderer() = default;
        virtual ~SystemRenderer() = default;
        
        /**
         * 渲染一个显示对象
         * @param displayObject 要渲染的显示对象
         * @param buffer 渲染缓冲
         * @param matrix 要叠加的矩阵
         * @param forRenderTexture 绘制目标是RenderTexture的标志
         * @returns drawCall触发绘制的次数
         */
        virtual int render(DisplayObject* displayObject, RenderBuffer* buffer, const Matrix& matrix, bool forRenderTexture = false) = 0;
        
        /**
         * 将一个RenderNode对象绘制到渲染缓冲
         * @param node 要绘制的节点
         * @param buffer 渲染缓冲
         * @param matrix 要叠加的矩阵
         * @param forHitTest 绘制结果是用于碰撞检测。若为true，当渲染GraphicsNode时，会忽略透明度样式设置，全都绘制为不透明的。
         */
        virtual void drawNodeToBuffer(RenderNode* node, RenderBuffer* buffer, const Matrix& matrix, bool forHitTest = false) = 0;
        
        /**
         * 清空渲染缓冲
         */
        virtual void renderClear() = 0;

        /**
         * 使渲染器缓存的位图数据失效（可选实现）
         */
        virtual void invalidateBitmapData(BitmapData* /*bmp*/) {}
    };

    // ========== 全局渲染器实例 ==========
    
    /**
     * 系统主渲染器（用于正常渲染）
     */
    extern std::unique_ptr<SystemRenderer> systemRenderer;
    
    /**
     * 画布渲染器（用于碰撞检测）
     */
    extern std::unique_ptr<SystemRenderer> canvasRenderer;

    // ========== 工厂方法 ==========
    
    /**
     * 初始化全局渲染器
     */
    void initializeRenderers();
    
    /**
     * 创建Skia渲染缓冲区
     */
    std::shared_ptr<RenderBuffer> createRenderBuffer(double width = 0, double height = 0);
    
    /**
     * 清理全局渲染器
     */
    void cleanupRenderers();

} // namespace sys
} // namespace egret
