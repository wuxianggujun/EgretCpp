/**
 * @file GroupNode.cpp
 * @brief GroupNode类实现 - 组渲染节点
 *
 * 翻译自：egret-core-5.4.1/src/egret/player/nodes/GroupNode.ts
 * GroupNode继承自RenderNode，用于组合多个渲染节点。
 */

#include "player/nodes/GroupNode.hpp"

namespace egret {
namespace sys {

// ========== GroupNode组节点实现 ==========

GroupNode::GroupNode()
    : RenderNode(RenderNodeType::GroupNode) {
    // 初始化已在头文件中完成（使用C++11默认成员初始化）
}

void GroupNode::render(void* renderer) {
    if (!renderer) {
        return;
    }
    
    m_renderCount++;
    
    // 渲染所有子节点
    for (auto& childNode : m_childNodes) {
        if (childNode) {
            childNode->render(renderer);
        }
    }
}

void GroupNode::addNode(std::shared_ptr<RenderNode> node) {
    // 完全对应TypeScript实现：
    // public addNode(node:RenderNode):void {
    //     this.drawData.push(node);
    // }
    
    if (node) {
        m_childNodes.push_back(node);
    }
}

void GroupNode::cleanBeforeRender() {
    // 完全对应TypeScript实现：
    // public cleanBeforeRender():void {
    //     let data = this.drawData;
    //     for (let i = data.length - 1; i >= 0; i--) {
    //         data[i].cleanBeforeRender();
    //     }
    // }
    
    // 注意：这里不调用父类的cleanBeforeRender()，按照TS源码的注释说明
    // "覆盖父类方法，不自动清空缓存的绘图数据，改为手动调用clear()方法清空。
    //  这里只是想清空绘制命令，因此不调用super"
    
    // 清理所有子节点
    for (auto& childNode : m_childNodes) {
        if (childNode) {
            childNode->cleanBeforeRender();
        }
    }
}

int GroupNode::getRenderCount() {
    // 完全对应TypeScript实现：
    // public $getRenderCount():number {
    //     let result = 0;
    //     let data = this.drawData;
    //     for (let i = data.length - 1; i >= 0; i--) {
    //         result += data[i].$getRenderCount();
    //     }
    //     return result;
    // }
    
    int result = 0;
    for (auto& childNode : m_childNodes) {
        if (childNode) {
            result += childNode->getRenderCount();
        }
    }
    return result;
}

} // namespace sys
} // namespace egret