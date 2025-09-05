/**
 * @file GroupNode.hpp
 * @brief GroupNode类 - 组渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/GroupNode.ts
 * GroupNode继承自RenderNode，用于组合多个渲染节点。
 */

#pragma once

#include "player/RenderNode.hpp"
#include "geom/Matrix.hpp"
#include <memory>
#include <vector>

namespace egret {
namespace sys {

/**
 * @brief GroupNode类 - 组渲染节点
 * 
 * GroupNode继承自RenderNode，用于组合多个渲染节点。
 * 完全基于TypeScript egret.sys.GroupNode实现，保持100%API兼容。
 * 
 * @note 对应TypeScript的egret.sys.GroupNode类
 */
class GroupNode : public RenderNode {
public:
    /**
     * @brief 构造函数
     */
    GroupNode();
    
    /**
     * @brief 析构函数
     */
    virtual ~GroupNode() = default;
    
    /**
     * @brief 渲染方法实现
     * @param renderer 渲染器实例
     */
    void render(void* renderer) override;
    
    /**
     * @brief 清理渲染前数据 - 重写父类方法
     * 
     * 对应 public cleanBeforeRender():void
     * 覆盖父类方法，不自动清空缓存的绘图数据，改为手动调用clear()方法清空。
     * 这里只是想清空绘制命令，因此不调用super。
     */
    void cleanBeforeRender() override;
    
    /**
     * @brief 获取渲染计数 - 对应 public $getRenderCount():number
     * 
     * @return 总的渲染计数（包括所有子节点）
     */
    int getRenderCount() override;

    // ========== 核心属性（对应TypeScript公共属性）==========
    
    /**
     * @brief 相对偏移矩阵 - 对应 public matrix: egret.Matrix;
     */
    std::shared_ptr<Matrix> matrix = nullptr;

    // ========== 公共方法 ==========
    
    /**
     * @brief 添加子节点 - 对应 public addNode(node:RenderNode):void
     * 
     * @param node 要添加的渲染节点
     */
    void addNode(std::shared_ptr<RenderNode> node);

private:
    /**
     * @brief 子节点列表
     * 
     * 存储所有子渲染节点，对应TypeScript中的drawData数组（用于存储子节点）
     */
    std::vector<std::shared_ptr<RenderNode>> m_childNodes;
};

} // namespace sys
} // namespace egret