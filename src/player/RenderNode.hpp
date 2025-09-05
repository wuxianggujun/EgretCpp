#pragma once
#include "core/HashObject.hpp"
#include <vector>

namespace egret {
namespace sys {

    // 前向声明
    class NormalBitmapNode;

    /**
     * 渲染节点类型枚举
     * 基于TypeScript egret.sys.RenderNodeType实现
     */
    enum class RenderNodeType {
        BitmapNode = 1,      // 位图渲染节点
        TextNode = 2,        // 文本渲染节点  
        GraphicsNode = 3,    // 矢量渲染节点
        GroupNode = 4,       // 组渲染节点
        MeshNode = 5,        // Mesh节点
        NormalBitmapNode = 6 // 普通位图渲染节点
    };
    
    /**
     * 渲染节点基类
     * 基于TypeScript egret.sys.RenderNode实现
     */
    class RenderNode : public HashObject {
    public:
        RenderNode(RenderNodeType type = RenderNodeType::BitmapNode);
        virtual ~RenderNode() = default;
        
        /**
         * 获取节点类型
         */
        RenderNodeType getType() const { return m_type; }
        
        /**
         * 获取绘制数据
         */
        std::vector<double>& getDrawData() { return m_drawData; }
        const std::vector<double>& getDrawData() const { return m_drawData; }
        
        /**
         * 清空绘制数据
         */
        virtual void cleanBeforeRender();
        
        /**
         * 渲染节点内容（纯虚函数，由子类实现）
         * @param renderer 渲染器实例
         */
        virtual void render(void* renderer) = 0;
        
        /**
         * 获取渲染计数（虚函数，可由子类重写）
         * @return 渲染计数
         */
        virtual int getRenderCount() { return m_renderCount; }
        
    protected:
        RenderNodeType m_type;              // 节点类型
        std::vector<double> m_drawData;     // 绘制数据数组
        int m_renderCount = 0;              // 绘制次数计数
    };
    
    // BitmapNode已移动到单独的文件 player/nodes/BitmapNode.hpp
    class BitmapNode; // 前向声明

    // TextNode已移动到单独的文件 player/nodes/TextNode.hpp
    class TextNode; // 前向声明

    // GroupNode已移动到单独的文件 player/nodes/GroupNode.hpp
    class GroupNode; // 前向声明

    // MeshNode已移动到单独的文件 player/nodes/MeshNode.hpp
    class MeshNode; // 前向声明

} // namespace sys
} // namespace egret