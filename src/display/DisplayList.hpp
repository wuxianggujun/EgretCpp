#pragma once
#include "core/HashObject.hpp"
#include "player/RenderNode.hpp"
#include <memory>
#include <vector>

namespace egret {
namespace sys {
    // 前向声明
    class RenderBuffer;

    /**
     * 显示列表 - 管理显示对象的渲染节点和绘制数据
     * 基于TypeScript egret.sys.DisplayList实现
     */
    class DisplayList : public HashObject {
    public:
        DisplayList();
        virtual ~DisplayList() = default;
        
        /**
         * 获取渲染节点
         */
        std::shared_ptr<RenderNode> getRenderNode() const { return m_renderNode; }
        
        /**
         * 设置渲染节点
         */
        void setRenderNode(std::shared_ptr<RenderNode> node) { m_renderNode = node; }
        
        /**
         * 绘制到表面
         * @returns 绘制调用次数
         */
        int drawToSurface();
        
        /**
         * 清空显示列表
         */
        void clear();
        
        /**
         * 检查是否为空
         */
        bool isEmpty() const { return !m_renderNode; }
        
        /**
         * 设置脏标记（需要重新渲染）
         */
        void setDirty(bool dirty = true) { m_dirty = dirty; }
        
        /**
         * 添加舞台渲染到表面的方法（用于Stage）
         */
        int stageRenderToSurface() { return drawToSurface(); }
        
        /**
         * 获取渲染缓冲区（用于Stage等特殊情况）
         */
        std::shared_ptr<RenderBuffer> getRenderBuffer() const { return m_renderBuffer; }
        
        /**
         * 设置渲染缓冲区
         */
        void setRenderBuffer(std::shared_ptr<RenderBuffer> buffer) { m_renderBuffer = buffer; }
        
        /**
         * 设置剪裁边界，不再绘制完整目标对象，画布尺寸由外部决定，超过边界的节点将跳过绘制
         * @param width 剪裁宽度
         * @param height 剪裁高度
         */
        void setClipRect(double width, double height) {
            m_clipWidth = width;
            m_clipHeight = height;
            m_hasClipRect = true;
        }
        
        /**
         * 获取剪裁宽度
         */
        double getClipWidth() const { return m_clipWidth; }
        
        /**
         * 获取剪裁高度  
         */
        double getClipHeight() const { return m_clipHeight; }
        
        /**
         * 是否有剪裁矩形
         */
        bool hasClipRect() const { return m_hasClipRect; }
        
    private:
        std::shared_ptr<RenderNode> m_renderNode;
        std::shared_ptr<RenderBuffer> m_renderBuffer;
        bool m_dirty = false;
        
        // 剪裁矩形相关
        double m_clipWidth = 0.0;
        double m_clipHeight = 0.0;
        bool m_hasClipRect = false;
    };

} // namespace sys
} // namespace egret